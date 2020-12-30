#pragma once

#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"

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
        void generatePredictions();

        std::string currentTime;

        OrderBook orderBook{"20200317.csv"};

        Wallet wallet;

};