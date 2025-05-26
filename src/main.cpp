#include <iostream>

#include "ComputerClub.h"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    ComputerClub club(argv[1]);
    return 0;
}