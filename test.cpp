#include <iostream>
#include <string>
#include <vector>
#include <fstream>

std::vector<std::string> tokenise(std::string csvLine, char separator)
{
    // string vector tokens ## stores the tokens
    std::vector<std::string> tokens;

    // int start, end ## used to indicate the position of the tokens
    signed int start, end;
    std::string token;

    start = csvLine.find_first_not_of(separator, 0);
    do
    {
        end = csvLine.find_first_of(separator, start);
        if (start == csvLine.length() || start == end)
        {
            break;
        }

        if( end>=0 )
        {
            token = csvLine.substr( start, end - start);
        }

        else
        {
            token = csvLine.substr(start, csvLine.length() - start);
        }
        
        // save the token
        tokens.push_back(token);
        // move past the current token.
        start = end + 1;

    } while(end > 0);
        return tokens;
    
}

int main()
{
    std::ifstream csvFile{"20200601.csv"};
    std::string line;
    std::vector<std::string> tokens;

    if(csvFile.is_open())
    {
        std::cout<< "File open" << std::endl;
        while(std::getline(csvFile, line))
        {
            std::cout << line << std::endl;
            tokens = tokenise(line, ',');
            
            //check if token size is 5 , if lesser t han 5 or more than 5 ignore.
            if(tokens.size() != 5)
            { 
                std::cout << "Bad line" << std::endl;
                continue;
            }
            
            try{
                // stod = string to double.
                double price = std::stod(tokens[3]);
                double amount = std::stod(tokens[4]);
                std::cout << price << ":" << amount << std::endl;

            }catch(std::exception& e) {
                std::cout << "Bad float! " << tokens[3] << std::endl;
                std::cout << "Bad float! " << tokens[4] << std::endl;
            }

            for(std::string& t : tokens)
            {
                std::cout << t << std::endl;
            }
        }
        
        csvFile.close();
    }

    else
    {
        std::cout << "File is not open" << std::endl;
    }

    return 0;
}