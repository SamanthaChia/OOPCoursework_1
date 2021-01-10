#include <fstream>
#include "OrderBookEntry.h"
#include "OrderBook.h"

class Logs{
    public:
        Logs();

        /** appends the wallet values in AssetsLog.csv
         * to observe the changes in assets.
         */
        void createAssetLogs(std::string currentTime, std::string wallet);

        /** appends all bids or ask offers made into AllSalesLog.csv 
         * contains type, name, price, amount, time
        */
        void createAllSalesLogs(std::string currentTime, OrderBookEntry obe);
        
        /** appends ONLY SUCCESSFUL bids or ask offers made into successfulSalesLog.csv
         * contains type, name, price, amount , time, average bid, average ask, cost
         */
        void createSuccessfulSalesLogs(std::string currentTime, OrderBookEntry sale, OrderBook orderBook);
        void ensureLogFilesEmpty();

        std::ofstream logBot;

};