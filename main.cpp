#include "common.h"
#include "window.h"

int main() {
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
    return 0;
}
