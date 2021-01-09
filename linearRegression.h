#pragma once
#include "DataHolder.h"
#include "OrderBook.h"


class linearRegression{
    public:
        linearRegression();

        /** call to run generateDataHolder for 10 times and generatePredictions 
         * for specified product
         */
        void generateDataHolder(std::string currentTime);

        /** called to run linear regression predictions using productData
        * calculations done here.
        */
        double generatePredictions(std::vector<DataHolder> productData);

       /** called to create data holders for specific products and also 
        * to obtain ask and bid entries. DataHolder containing
        * productName, averageAsk, askVolume, averageBid, bidVolume
        * is created and pushed_back here
        */
        void automatePredictionBot();

        // std::string currentTime;
        std::vector<DataHolder> btcUSDTDataHolder;
        std::vector<DataHolder> dogeBTCDataHolder;
        std::vector<DataHolder> dogeUSDTDataHolder;
        std::vector<DataHolder> ethBTCDataHolder;
        std::vector<DataHolder> ethUSDTDataHolder;

        OrderBook orderBook{"20200601.csv"};
};