#include "common.h"
#include "chip.h"
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Suppose to run with rom file" << std::endl;
        return 1;
    }

    std::string romPath = argv[1];

    Chip8 chip;

    if(!chip.loadROM(romPath)) {
        std::cerr << "Failed to load rom" << std::endl;
    }
    std::cout << "Loaded rom" << std::endl;

    while(true) {
        chip.fetch();
        chip.decodeAndExecute();
        if(!chip.render()) {
            break;
        }
        sleep(1);
    }
    return 0;
}
