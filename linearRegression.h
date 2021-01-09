#pragma once
#include "DataHolder.h"
#include "OrderBook.h"
#include <vector>
#include <string>

class LinearRegression{
    public:
        LinearRegression();

        /** called to create data holders for specific products.
         * DataHolder containing, productName, averageAsk, askVolume, averageBid, bidVolume
        * is created here for the current TimeStamp.
        */
        void generateDataHolder(std::string currentTime, OrderBook orderBook);

        /** called to run linear regression predictions using productData
         * calculations done here.
        */
        double generatePredictions(std::vector<DataHolder> productData);        


        std::vector<DataHolder> btcUSDTDataHolder;
        std::vector<DataHolder> dogeBTCDataHolder;
        std::vector<DataHolder> dogeUSDTDataHolder;
        std::vector<DataHolder> ethBTCDataHolder;
        std::vector<DataHolder> ethUSDTDataHolder;

};