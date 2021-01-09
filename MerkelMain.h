#pragma once

#include <vector>
#include <fstream>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "PredictB0B1.h"
#include "DataHolder.h"
#include "LinearRegression.h"


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

        /** call to run generateDataHolder for 10 times and generatePredictions 
         * for specified product
         */
        void automatePredictionBot();
        void checkEligibleOrder();
        void generateBidWithPredictions(std::string productName, double predictedVal);
        void generateOfferWithPredictions(std::string productName, double predictedVal);
        void createAssetLogs();
        void createAllSalesLogs(OrderBookEntry obe);
        void createSuccessfulSalesLogs(OrderBookEntry sale);
        void ensureLogFilesEmpty();


        std::string currentTime,nextCurrentTime;
        std::ofstream logBot;


        OrderBook orderBook{"20200601.csv"};

        Wallet wallet;

        LinearRegression linearRegressionPrediction;
};