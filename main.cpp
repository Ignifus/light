#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>

std::string readInput(const std::string& filepath){

    std::ifstream in(filepath, std::ios::in | std::ios::binary);
    if (in) {
        if(filepath.substr(filepath.find_last_of(".") + 1) != "l")
            throw std::runtime_error("Invalid input extension. Not a .l file.");

        std::ostringstream contents;
        contents << in.rdbuf();
        in.close();
        return contents.str();
    }

    throw std::runtime_error("Input file not found.");
}

void interpret(const std::string& data){
    std::cout << data << std::endl;
}

int main(int argc, const char * argv[]) {
    try {
        if (argc > 1) {
            interpret(readInput(argv[1]));
        } else {
            throw std::runtime_error("No input file was specified in paramters.");
        }
    }
    catch(std::runtime_error e){
        std::cerr << e.what() << std::endl;
    }
}