#pragma once

#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>

class OrderBook
{
    public:
        /** construct, readying a csv data file */
        OrderBook(std::string filename);
        /** return vector of all known products in the dataset */
        std::vector<std::string> getKnownProducts();
        /** return vector of Orders according to the sent filters */
        std::vector<OrderBookEntry> getOrders(OrderBookType type, std::string product, std::string timestamp);

        /** return earliest time in orderbook */
        std::string getEarliestTime();

        /** return next time after sent time in orderbook
         * if there is no next timestamp, starts from the beginning again.
         */
        std::string getNextTime(std::string timestamp);

        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        static double getTotalPrice(std::vector<OrderBookEntry>& orders);

        /** call this function to remove any unmatched salles from previous time frames*/
        void removeUnmatchedSales(OrderBook orderBook, std::string previousTimeFrame);
        
    
    private:
        std::vector<OrderBookEntry> orders;

};