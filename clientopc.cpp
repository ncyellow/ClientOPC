#include "clientopc.h"
#include "server.h"

#include <QTimer>
#include <QDebug>
#include <iostream>
#include "configreader.h"

ClientOPC::ClientOPC(QObject *parent) : QObject(parent), server(0)
{
    timer = new QTimer(this);
    timer->setInterval(30*1000);
}

void ClientOPC::createServer()
{
    ConfigReader conf;
    conf.readSettings();

    server = new Server(conf.getHost(), conf.getProgId(), this);
    server->createGroup();
    server->addItems(conf.getItems());
    connect(timer, SIGNAL(timeout()), server, SLOT(pollItems()));
}

void ClientOPC::startPoll()
{
    if(!timer->isActive())
    {
        timer->start();
        std::cout << "> poll started" << std::endl << "> ";
    }
    else
    {
        std::cout << "> poll already started" << std::endl << "> ";
    }
}

void ClientOPC::stopPoll()
{
    if(timer->isActive())
    {
        timer->stop();
        std::cout << "> poll stopped" << std::endl << "> ";
    }
    else
    {
        std::cout << "> poll already stopped" << std::endl << "> ";
    }
}


ClientOPC::~ClientOPC()
{
}

