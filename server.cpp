#include "server.h"

#include <iostream>
#include <vector>
#include <QVariant>
#include <QDebug>

Server::Server(const std::wstring &host, const std::wstring &progID, QObject *parent):
    QObject(parent),
    m_host(host),
    m_progID(progID),
    m_hServer(0),
    m_ipGroup(NULL),
    m_ipSyncIO(NULL),
    m_server(createServer()),
    m_serverHandles(NULL)
{

}

Server::~Server()
{
    CoTaskMemFree(m_serverHandles);
    m_ipSyncIO = NULL;
    m_ipGroup = NULL;
    m_server = NULL;
    ::CoUninitialize();
}

OPCSERVERSTATE Server::getStatus() const
{
    if(m_server == NULL) return OPC_STATUS_FAILED;

    OPCSERVERSTATUS *status_ptr(NULL);
    HRESULT result = m_server->GetStatus(&status_ptr);

    OPCSERVERSTATE status = OPC_STATUS_FAILED;
    if(SUCCEEDED(result))
    {
        status = status_ptr->dwServerState;
        CoTaskMemFree(status_ptr);
    }
    return OPC_STATUS_FAILED;
}


void Server::addItems(const std::vector<ItemOPC> &arg)
{
    if(m_server == NULL) return;
    items = arg;
    HRESULT result;
    DWORD dwCount(items.size());
    OPCITEMDEF* pItems =  (OPCITEMDEF*)CoTaskMemAlloc(dwCount*sizeof(OPCITEMDEF));
    OPCITEMRESULT* pResult(NULL);
    HRESULT* pErrors(NULL);
    memset(pItems, 0, dwCount*sizeof(OPCITEMDEF));

    for(int offset = 0 ; offset < arg.size(); ++offset)
    {
        pItems[offset].szItemID = (LPWSTR)arg.at(offset).itemId.data();
        pItems[offset].szAccessPath = NULL;
        pItems[offset].bActive = TRUE;
        pItems[offset].hClient = 0;
        pItems[offset].vtRequestedDataType = VT_EMPTY;
        pItems[offset].dwBlobSize = 0;
        pItems[offset].pBlob = NULL;
    }

    result = m_ipGroup->AddItems(dwCount, pItems, &pResult, &pErrors);
    if(FAILED(result))
    {
        std::cout << "Add items failed" << std::endl;
        std::cout.flush();
    }
    if(result == S_FALSE)
    {
        std::cout << "Add items failed" << std::endl;
        std::cout.flush();
    }
    if(result == S_OK)
    {
        m_hServer = pResult->hServer;
        m_serverHandles = (OPCHANDLE*)CoTaskMemAlloc(dwCount*sizeof(OPCHANDLE));
        for(int offset = 0 ; offset < arg.size(); ++offset)
        {
            m_serverHandles[offset] = pResult[offset].hServer;
            if(pResult[offset].dwBlobSize > 0)
                CoTaskMemFree(pResult[offset].pBlob);
        }
    }


    CoTaskMemFree(pItems);
    CoTaskMemFree(pResult);
    CoTaskMemFree(pErrors);
}

CComPtr<IOPCServer> Server::createServer()
{
    ::CoInitialize(NULL);
    CComPtr<IOPCServerList> ipServerList = NULL;
    CComPtr<IOPCServer> ipServer = NULL;

    COSERVERINFO ServerInfo = {0};
    ServerInfo.pwszName = (LPWSTR)m_host.data();
    ServerInfo.pAuthInfo = NULL;

    MULTI_QI MultiQI [1] = {0};

    MultiQI [0].pIID = &IID_IOPCServerList;
    MultiQI [0].pItf = NULL;
    MultiQI [0].hr = S_OK;

    HRESULT result = CoCreateInstanceEx (CLSID_OpcServerList, NULL, CLSCTX_REMOTE_SERVER, &ServerInfo, 1, MultiQI);
    CLSID opcServerId;
    if (result == S_OK)
    {
        ipServerList = static_cast<IOPCServerList*>(MultiQI[0].pItf);

        if(ipServerList->CLSIDFromProgID(m_progID.data(), &opcServerId) == S_OK)
        {
            MultiQI[0].pIID = &IID_IOPCServer;
            MultiQI[0].pItf = NULL;
            MultiQI[0].hr = S_OK;

            if (CoCreateInstanceEx (opcServerId, NULL, CLSCTX_REMOTE_SERVER, &ServerInfo, 1, MultiQI) == S_OK)
            {
                ipServer = static_cast<IOPCServer*>(MultiQI[0].pItf);
            }
        }
    }
    return ipServer;
}

void Server::pollItems()
{
    if(m_server == NULL) return;
    OPCITEMSTATE *st;
    HRESULT *Error(NULL);
    m_ipSyncIO->Read(OPC_DS_CACHE, items.size(), m_serverHandles, &st, &Error);

    QVariantMap values;

    for(int i = 0; i < items.size(); ++i)
    {
        QString tagname = QString::fromStdWString(items.at(i).itemId);
        OPCITEMSTATE *temp = st + i;
        switch (temp->vDataValue.vt) {
        case VT_BOOL:
            values.insert(tagname, temp->vDataValue.boolVal);
            break;
        case VT_BSTR:
        {
            QString tmp((QChar*)temp->vDataValue.bstrVal, ::SysStringLen(temp->vDataValue.bstrVal));
            values.insert(tagname, tmp);
            break;
        }
        default:
            break;
        }
    }
    qDebug() << values;

    CoTaskMemFree(Error);
    CoTaskMemFree(st);
}

void Server::createGroup()
{
    if(m_server == NULL) return;
    OPCHANDLE hGroup(NULL);
    HRESULT result(NULL);
    DWORD dwRevisedUpdateRate(0);
    result = m_server->AddGroup(L"SmisGroup",
                              TRUE,
                              1000,
                              NULL,
                              NULL,
                              NULL,
                              LOCALE_SYSTEM_DEFAULT, &hGroup, &dwRevisedUpdateRate, IID_IOPCItemMgt, (LPUNKNOWN*)&m_ipGroup);
    if(FAILED(result))
    {
        std::cout << "AddGroup failed" << std::endl;
        std::cout.flush();
    }
    m_ipGroup->QueryInterface(IID_IOPCSyncIO, (void**)&m_ipSyncIO);
}

