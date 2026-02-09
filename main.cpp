#include "common.h"
#include "chip.h"
#include <iomanip>

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

    /*
    WindowManager window;
    std::cout << "Begin Initialize" << std::endl;
    if(!window.initialize()) {
        return 3;
    }

    std::cout << "Begin Rendering" << std::endl;
    while (1) {
        // window.addSquare(i,i);
        if (!window.render()) {
            break;
        }
    }

    std::cout << "Begin Destroy" << std::endl;
    window.destroy();
    */
    return 0;
}
