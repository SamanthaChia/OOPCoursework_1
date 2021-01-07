#pragma once

#include "OrderBookEntry.h"
#include <string>
#include <vector>

class CSVCreator{
    public:
        CSVCreator();
        void creatCSV(std::string csvFileName);
};