#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

std::map<std::string, int> integers;
std::map<std::string, bool> booleans;
std::map<std::string, std::string> strings;

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

int solveInt(const std::string& declaration){
    std::string unprocessedOperations = std::regex_replace(declaration, std::regex("[\\s]"), "");
    int result = 0;

    unsigned long operationSeparator = unprocessedOperations.find_first_of("+");

    // Is single declaration?
    if(operationSeparator == -1ul)
    {
        try{
            result = std::stoi(unprocessedOperations);
        }
        catch(std::exception e){
            result = integers[unprocessedOperations];
        }

        return result;
    }

    while(operationSeparator != -1ul){
        // Get the next operation separator
        unsigned long nextOperationSeparator = -1ul;
        for (unsigned long i = operationSeparator + 1ul; i < unprocessedOperations.length(); ++i) {
            if(unprocessedOperations[i] == '+'){
                nextOperationSeparator = i;
                break;
            }
        }

        // Get the isolated operation
        std::string operation = unprocessedOperations.substr(0, nextOperationSeparator != -1ul ? nextOperationSeparator : unprocessedOperations.length());

        // Get left operation part
        std::string left = trim(operation.substr(0, operation.find_first_of("+")));

        int leftResult = 0;

        try{
            leftResult = std::stoi(left);
        }
        catch(std::exception e){
            leftResult = integers[left];
        }

        int rightResult = 0;

        // If not single part, get right part
        if(operation.find_first_of("+") != -1ul){
            std::string right = trim(operation.substr(operation.find_first_of("+") + 1, operation.length()));

            try{
                rightResult = std::stoi(right);
            }
            catch(std::exception e){
                rightResult = integers[right];
            }
        }

        result += leftResult + rightResult;

        operationSeparator = nextOperationSeparator;
        if(nextOperationSeparator != -1ul)
            unprocessedOperations = unprocessedOperations.substr(nextOperationSeparator + 1, unprocessedOperations.length());
    }

    return result;
}

bool solveBool(const std::string& declaration){
    std::string unprocessedOperation = std::regex_replace(declaration, std::regex("[\\s]"), "");

    if(unprocessedOperation=="si")
        return true;
    else if(unprocessedOperation=="no")
        return false;

    std::string left = trim(unprocessedOperation.substr(0, unprocessedOperation.find_first_of("==")));
    std::string right = trim(unprocessedOperation.substr(unprocessedOperation.find_first_of("==") + 2, unprocessedOperation.length()));

    int leftOperation, rightOperation;

    if(left=="si")
        leftOperation = 1;
    else if(left=="no")
        leftOperation = 0;
    else
        leftOperation = solveInt(left);

    if(right=="si")
        rightOperation = 1;
    else if(right=="no")
        rightOperation = 0;
    else
        rightOperation = solveInt(right);

    return leftOperation==rightOperation;
}

void interpret(const std::string& data);
void processStructure(const std::string& structureHeader, const std::string& structureContent){
    if(structureHeader.find("if") != -1ul){
        if(solveBool(structureHeader.substr(2, structureHeader.length()))){
            interpret(structureContent);
        }
    }
    else if(structureHeader.find("while") != -1ul){
        while(solveBool(structureHeader.substr(5, structureHeader.length()))){
            interpret(structureContent);
        }
    }
}

void processStatement(const std::string& statement){
    std::string type = statement.substr(0,statement.find_first_of(" "));
    std::string identifier = trim(statement.substr(statement.find_first_of(" "), statement.find_first_of("=")-statement.find_first_of(" ")));
    std::string declaration = trim(statement.substr(statement.find_first_of("=") + 1, statement.length()));

    for(auto& var : integers){
        if(var.first == type)
            var.second = solveInt(declaration);
    }

    for(auto& var : booleans){
        if(var.first == type)
            var.second = solveBool(declaration);
    }

    if(type == "numerito")
        integers[identifier] = solveInt(declaration);
    else if(type == "buleano")
        booleans[identifier] = solveBool(declaration);
    else if(type == "palabrita")
        strings[identifier] = declaration.substr(declaration.find_first_of("\"") + 1, declaration.find_last_of("\"") - 1);
}

void interpret(const std::string& data){
    //Erase all tabs and newlines
    std::string unprocessedStatements = std::regex_replace(data, std::regex("[\\n]|[\\t]"), "");

    //Get first structure opening
    unsigned long structureStart = unprocessedStatements.find_first_of("{");

    //Get first statement ending
    unsigned long statementEnding = unprocessedStatements.find_first_of(";");

    // If there are statements left in program, keep on processing, ignore empty structures
    while(statementEnding != -1ul){

        // Is structure before statements?
        while(structureStart != -1ul && structureStart < statementEnding){

            std::string structureHeader = unprocessedStatements.substr(0, structureStart);
            unprocessedStatements = unprocessedStatements.substr(structureStart + 1);

            unsigned long structureEnding = unprocessedStatements.find_first_of("}");
            std::string structureContent = unprocessedStatements.substr(0, structureEnding);

            processStructure(trim(structureHeader), trim(structureContent));

            unprocessedStatements = unprocessedStatements.substr(structureEnding + 1);

            // Get next structure start
            structureStart = unprocessedStatements.find_first_of("{");
        }

        //Get the next statement ending
        statementEnding = unprocessedStatements.find_first_of(";");

        //Get the following unprocessed statement
        std::string statement = unprocessedStatements.substr(0, statementEnding);

        //Process the statement
        processStatement(trim(statement));

        //Update the remaining statements
        unprocessedStatements = unprocessedStatements.substr(statementEnding+1);

        //Get the next statement ending
        statementEnding = unprocessedStatements.find_first_of(";");

        // Get next structure start
        structureStart = unprocessedStatements.find_first_of("{");
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
            for(auto in : booleans){
                std::cout << in.first << " " << in.second << std::endl;
            }
            for(auto in : strings){
                std::cout << in.first << " " << in.second << std::endl;
            }
        } else {
            throw std::runtime_error("No input file was specified in parameters.");
        }
    }
    catch(const std::exception& e){
        std::cerr << "Error when executing: " << e.what() << std::endl;
    }
}