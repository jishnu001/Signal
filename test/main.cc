#include "../Signal.h"
#include "SomeClass.h"
#include <unistd.h>

int main()
{
    SomeClass y(1);
    SomeClass y2(2);

    // Timeout t(2, false); // 10 second timeout  not singleshot
    // Connect(t, timeout, y, SomeClass::listen);
    // t.start();

    Timeout t(1, false); // 10 second timeout, not singleshot
    Connect(t, Timeout::timeoutWithData, y2, SomeClass::listenMore);
    Connect(t, Timeout::timeoutWithData, y, SomeClass::listenMore);
    /*
    auto fn = [=](const std::string &v) { std::cerr << "\n ** " << v << " **\n"; };
    auto fn1 = [=](const std::string &v) { std::cerr << "\n ** 1 " << v << " **\n"; };

    ConnectL(t, Timeout::timeoutWithData, fn);
    ConnectL(t, Timeout::timeoutWithData, fn1);

    auto fn = [=]() {
        std::cerr << "\n ** "
                  << "TMED OUT"
                  << " **\n";
    };
    ConnectL(t, Timeout::timeout, fn);*/

    t.start();

    while (1)
    {
        std::cerr << "\n.....\n";
        sleep(4);
        // DisconnectL(t, Timeout::timeoutWithData, fn);
    }
    return 0;
}