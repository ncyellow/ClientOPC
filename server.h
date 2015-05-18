#ifndef SERVER_H
#define SERVER_H


#include <QObject>
#include "opc/OpcEnum.h"
#include "opc/opcda.h"
#include <atlbase.h>
#include <vector>
#include <list>
#include <boost/variant.hpp>

struct ItemOPC
{
    ItemOPC(std::wstring id):itemId(id){}
    std::wstring itemId;
};

class Server: public QObject
{
    Q_OBJECT
public:
    ~Server();

    OPCSERVERSTATE getStatus() const;
    void createGroup();
    void addItems(const std::vector<ItemOPC> &arg);
    Server(const std::wstring &host, const std::wstring &progID, QObject* parent = 0);
private:
    CComPtr<IOPCServer> createServer();
public slots:
    void pollItems();
private:
    const std::wstring m_host;
    const std::wstring m_progID;
    OPCHANDLE m_hServer;
    CComPtr<IOPCItemMgt>m_ipGroup;
    CComPtr<IOPCSyncIO> m_ipSyncIO;
    CComPtr<IOPCServer> m_server;
    OPCHANDLE* m_serverHandles;
    std::vector<ItemOPC> items;
};

#endif // SERVER_H
