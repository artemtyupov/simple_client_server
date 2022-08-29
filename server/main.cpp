#include "common.hpp"
#include "server.hpp"

int main()
{
    CServer server{SERVER_IP, SERVER_PORT, SERVER_THREADS};
    server.Start();
    return EXIT_SUCCESS;
}
