#include <iostream>

int main() {
    // Flush after every std::cout / std:cerr
    std::cout << std::unitbuf;
    std::cerr << std::unitbuf;

    // Uncomment this block to pass the first stage

    while (true) {
        std::cout << "$ ";
        std::string input;
        std::getline(std::cin, input);
        if (input.starts_with("exit")) {
            return std::stoi(input.substr(4));
            break;
        }
        std::cout << input << ": command not found" << std::endl;
    }

    return 0;
}
