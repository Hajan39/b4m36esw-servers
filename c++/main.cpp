#include <iostream>
#include "main.h"

/**
 * Starts the word counting server.
 */
int main() {
    std::cout << "Disappointing C++ Server (libmicrohttpd, zlib, pthread) v0.3" << std::endl;
    std::cout << "(c) Filip Klimes, 2017" << std::endl;

    init_dictionary();
    if (start_server() > 0) {
        std::cerr << "Could not start server." << std::endl;
        return 1;
    }

    return 0;
}
