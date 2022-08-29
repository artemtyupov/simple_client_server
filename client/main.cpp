#include "common.hpp"
#include "client.hpp"

int main()
{
    CClient client{SERVER_IP, SERVER_PORT};
    if (!client.ConnectViaSocket())
        return EXIT_FAILURE;

    int a = 123;
    client.TransferDataToServer<int>(a);

    double b = 123.123;
    client.TransferDataToServer<double>(b);

    client.DisconnectFromSocket();

    return EXIT_SUCCESS;
}
