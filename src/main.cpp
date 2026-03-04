#include "server/TcpServer.h"

int main()
{

    TcpServer server(7379, 4);

    server.run();

    return 0;
}