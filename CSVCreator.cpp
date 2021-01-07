#include "CSVCreator.h"
#include <iostream>
#include <fstream>

CSVCreator::CSVCreator(){

}

void CSVCreator::createCSV(std::string csvFileName){
    
    std::fstream csvFile{csvFileName};

    if(csvFile.is_open()){
        csvFile << "Assets at ";
    } else
    {
        std::ofstream csvFile{csvFileName};
    }
}