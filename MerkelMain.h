#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
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
        void generatePredictions(std::vector<DataHolder> productData);
        void generateDataHolder();
        void automatePredictionBot();
        bool minAbsVal(double a, double b);

        std::string currentTime;
        std::vector<DataHolder> btcUSDTDataHolder;
        std::vector<DataHolder> dogeBTCDataHolder;
        std::vector<DataHolder> dogeUSDTDataHolder;
        std::vector<DataHolder> ethBTCDataHolder;
        std::vector<DataHolder> ethUSDTDataHolder;

        OrderBook orderBook{"20200601.csv"};

        Wallet wallet;

};