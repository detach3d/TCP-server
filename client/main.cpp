#include <exception>
#include "client.h"
#include <iostream>

int main(int argc, char *argv[]){

    try
    {
        client(argc, argv);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}