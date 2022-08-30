#include "common.hpp"
#include "client.hpp"

int main()
{
    CClient client{SERVER_IP, SERVER_PORT};
    int a = 12;
    client.TransferDataToServer<int>(a);

    double b = 12.12;
    client.TransferDataToServer<double>(b);

    return EXIT_SUCCESS;
}
