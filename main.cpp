#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

std::map<std::string, int> integers;

std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

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

void processStatement(const std::string& statement){
    std::string type = statement.substr(0,statement.find_first_of(" "));
    std::string identifier = trim(statement.substr(statement.find_first_of(" "), statement.find_first_of("=")-statement.find_first_of(" ")));
    std::string declaration = trim(statement.substr(statement.find_first_of("=") + 1, statement.length()));

    if(type == "numerito")
        integers.emplace(identifier, std::stoi(declaration));
}

void interpret(const std::string& data){
    //Erase all tabs and newlines
    std::string unprocessedStatements = std::regex_replace(data, std::regex("[\\n]|[\\t]"), "");

    //Get first statement ending
    unsigned long statementEnding = unprocessedStatements.find_first_of(";");

    while(statementEnding != -1ul){
        //Get the following unprocessed statement
        std::string statement = unprocessedStatements.substr(0, statementEnding);

        //Process the statement
        processStatement(trim(statement));

        //Update the remaining statements
        unprocessedStatements = unprocessedStatements.substr(statementEnding+1);

        //Get the next statement ending
        statementEnding = unprocessedStatements.find_first_of(";");
    }
}

int main(int argc, const char * argv[]) {
    try {
        if (argc > 1) {
            interpret(readInput(argv[1]));

            //Print the saved variables
            for(auto in : integers){
                std::cout << in.first << " " << in.second << std::endl;
            }
        } else {
            throw std::runtime_error("No input file was specified in parameters.");
        }
    }
    catch(std::runtime_error e){
        std::cerr << e.what() << std::endl;
    }
}