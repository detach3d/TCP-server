#include <exception>
#include "server.h"
#include <iostream>

int main(){

    try
    {
        server();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}