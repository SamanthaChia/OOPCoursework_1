#pragma once

#include <fstream>
#include "OrderBookEntry.h"

class Logs{
    public:
        Logs();

        void createAssetLogs(std::string currentTime, std::string wallet);
        void createAllSalesLogs(OrderBookEntry obe);
        void createSuccessfulSalesLogs(OrderBookEntry sale);
        void ensureLogFilesEmpty();

        std::ofstream logBot;

};