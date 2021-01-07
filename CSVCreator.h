#pragma once

#include "OrderBookEntry.h"
#include <string>
#include <vector>

class CSVCreator{
    public:
        CSVCreator();
        static void createCSV(std::string csvFileName);
};