#include <QCoreApplication>
#include <QFutureWatcher>
#include <QSharedPointer>
#include <QtConcurrentRun>
#include <QMetaMethod>

#include <iostream>
#include "clientopc.h"
#include "server.h"

void printCicle(ClientOPC *client)
{
    forever{
        QTextStream s(stdin);
        QString value = s.readLine();

        if(value == "quit")
        {
            return;
        }
        else if(value == "start")
        {
            QMetaObject::invokeMethod(client, "startPoll", Qt::QueuedConnection);
        }
        else if(value == "stop")
        {
            QMetaObject::invokeMethod(client, "stopPoll", Qt::QueuedConnection);
        }
        else
        {
            std::cout << "> avaliable commands - start | stop | quit" << std::endl << "> ";
        }
    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    ClientOPC client;
    client.createServer();
    client.startPoll();

    QFutureWatcher<void> watcher;
    QFuture<void> future = QtConcurrent::run(printCicle, &client);
    QObject::connect(&watcher, SIGNAL(finished()), &a, SLOT(quit()));
    watcher.setFuture(future);


    int result = a.exec();
    return result;
}
