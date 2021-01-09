#pragma once

#include <fstream>
#include "OrderBookEntry.h"

class Logs{
    public:
        Logs();

        void createAssetLogs();
        void createAllSalesLogs(OrderBookEntry obe);
        void createSuccessfulSalesLogs(OrderBookEntry sale);
        void ensureLogFilesEmpty();

        std::ofstream logBot;

};