#pragma once

#include <fstream>
#include "OrderBookEntry.h"
#include "OrderBook.h"

class Logs{
    public:
        Logs();

        void createAssetLogs(std::string currentTime, std::string wallet);
        void createAllSalesLogs(std::string currentTime, OrderBookEntry obe);
        void createSuccessfulSalesLogs(std::string currentTime, OrderBookEntry sale, OrderBook orderBook);
        void ensureLogFilesEmpty();

        std::ofstream logBot;

};