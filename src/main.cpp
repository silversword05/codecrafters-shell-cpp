#include "Commands.h"

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    std::string input;
    std::cout << "$ ";

    while (std::getline(std::cin, input)) {
        CommandProcessor command_processor;
        command_processor.dispatch(input);
        std::cout << "$ ";
    }

    return 0;
}
