#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "PredictB0B1.h"
#include "DataHolder.h"

class MerkelMain{

    public:
        MerkelMain();
        /** call this to start the sim */
        void init();
    
    private:
        void loadOrderBook();
        void printMenu();
        int getUserOption();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeFrame();
        void procesUserOption(int userOption);
        /** called to run linear regression predictions using productData
         * calculations done here.
        */
        double generatePredictions(std::vector<DataHolder> productData);
       /** called to create data holders for specific products and also 
        * to obtain ask and bid entries. DataHolder containing
        * productName, averageAsk, askVolume, averageBid, bidVolume
        * is created and pushed_back here
        */
        void generateDataHolder();
        /** call to run generateDataHolder for 10 times and generatePredictions 
         * for specified product
         */
        void automatePredictionBot();
        void checkEligibleOrder();
        void generateBidWithPredictions(std::string productName, double predictedVal);
        void generateOfferWithPredictions(std::string productName, double predictedVal);


        std::string currentTime,nextCurrentTime;
        std::vector<DataHolder> btcUSDTDataHolder;
        std::vector<DataHolder> dogeBTCDataHolder;
        std::vector<DataHolder> dogeUSDTDataHolder;
        std::vector<DataHolder> ethBTCDataHolder;
        std::vector<DataHolder> ethUSDTDataHolder;

        OrderBook orderBook{"20200601.csv"};

        Wallet wallet;
        CSVCreator csvCreator;
};