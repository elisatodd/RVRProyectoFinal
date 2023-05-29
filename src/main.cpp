#include <iostream>
#include "GameUtils/GameManager.h"
#include "SDLUtils/Window.h"
#include "Client/TronClient.h"

void initGame(char **argv){

    TronClient client(argv[1], argv[2]);
    client.init(900, 700);

    client.run();

    client.shutdown();
}

int main(int argc, char *argv[]){
    try {
        if(argc < 3){
            std::cout << "Usage: ./Tron.out <ip>(0.0.0.0) <port>(2000)\n";
            return 0;
        }
        initGame(argv);
    }
    catch(const std::string& e)
    {
        std::cerr << e << '\n';
    }
    catch(const char* e)
    {
        std::cerr << e << '\n';
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    catch(...)
    {
        std::cerr << "Caught an unknown exception ...\n";
    }
    
    return 0;
}