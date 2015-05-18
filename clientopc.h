#ifndef CLIENTOPC_H
#define CLIENTOPC_H

#include <QObject>

class QTimer;
class Server;
class ClientOPC : public QObject
{
    Q_OBJECT
public:
    explicit ClientOPC(QObject *parent = 0);
    void createServer();
    ~ClientOPC();
public slots:
    void startPoll();
    void stopPoll();
private:
    Server* server;
    QTimer* timer;
};

#endif // CLIENTOPC_H
