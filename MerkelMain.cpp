#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "DataHolder.h"
#include "linearRegression.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <algorithm>

MerkelMain::MerkelMain(){

}

void MerkelMain::init(){
    
    int input;
    currentTime = orderBook.getEarliestTime();
    ensureLogFilesEmpty();
    wallet.insertCurrency("BTC", 10);
    while(true){
        printMenu();
        input = getUserOption();
        procesUserOption(input);
    }
}

void MerkelMain::printMenu(){

    std::cout << "Welcome to MerkelrexBot!" <<std::endl;
    std::cout << "The aim of the bot is to make money. To analyse bids and make offers" << std::endl;

    std::cout << "================ " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

int MerkelMain::getUserOption()
{
    int userOption;
    std::string line;
    std::cout << "To start the bot, please enter 1" <<std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }
    catch(const std::exception& e){
        //
    }

    std::cout << "You have entered : " << userOption << std::endl;
    return userOption;
}

void MerkelMain::printHelp(){
    std::cout << "Help - your aim is to make money. Analyse bids and make offers" << std::endl;
}

void MerkelMain::printMarketStats(){
    for(std::string const& p : orderBook.getKnownProducts()){
        
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime );
        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, p, currentTime );
        
        std::cout <<"Bids seen : " << bidEntries.size() << std::endl;
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    
    }
}

void MerkelMain::enterAsk(){
    std::cout << "Make an ask - enter the amount: product, price, amount, eg ETH/BTC, 200,0.5 " << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input,',');
    if(tokens.size() != 3){
        std::cout << "MerkelMain::enterAsk Bad input " << std::endl;
    }

    else{
        try{
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::ask
            );
            obe.username = "simuser";

            if(wallet.canFulfillOrder(obe))
            {
                std::cout<<"Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else{
                std::cout<< "Wallet has insufficient funds. " << std::endl;
            }
        }catch(const std::exception& e)
        {
            std::cout << "MerkelMain::enterAsk Bad input " << std::endl;
        }
    }
}

void MerkelMain::enterBid(){
    std::cout << "Make an bid - enter the amount: product, price, amount, eg ETH/BTC, 200,0.5 " << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input,',');
    if(tokens.size() != 3){
        std::cout << "MerkelMain::enterBid Bad input " << std::endl;
    }

    else{
        try{
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2],
                currentTime,
                tokens[0],
                OrderBookType::bid
            );
            obe.username = "simuser";

            if(wallet.canFulfillOrder(obe))
            {
                std::cout<<"Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else{
                std::cout<< "Wallet has insufficient funds. " << std::endl;
            }
        }catch(const std::exception& e)
        {
            std::cout << "MerkelMain::enterBid Bad input " << std::endl;
        }
    }

}

void MerkelMain::printWallet(){
    std::cout << wallet.toString() << std::endl;
}

void MerkelMain::gotoNextTimeFrame(){
    std::cout << "Continue to next time step." << std::endl;
    for(std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for(OrderBookEntry& sale : sales)
        {   
            std::cout << "Sale amount : " << sale.price << " amount " << sale.amount << std::endl;
            if(sale.username == "simuser")
            {
                wallet.processSale(sale);
                createSuccessfulSalesLogs(sale);
            }
        }
    }
    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelMain::procesUserOption(int userOption){
    if(userOption <= 0 || userOption > 1){
        std::cout << "You have entered an invalid option." << std::endl;
    }
    else if(userOption == 1){
        std::cout << "Starting MerkelrexBot " << std::endl;
        //run 10 times so current time is 11:58:20.377111
        while(true){        
            automatePredictionBot();
            createAssetLogs();
            printWallet();
            checkEligibleOrder();
            gotoNextTimeFrame();
            nextCurrentTime = orderBook.getNextTime(currentTime);
            std::cout << "================ " << std::endl;
            std::cout << "Current time is: " << currentTime << std::endl;
            if(nextCurrentTime == orderBook.getEarliestTime()){
                break;
            }
        }
    }
}

void MerkelMain::automatePredictionBot(){
    //after 10 times
    for(int i =0; i<10;){
        linearRegressionPrediction.generateDataHolder(currentTime);
        currentTime = orderBook.getNextTime(currentTime);
        i++;
    }
}

void MerkelMain::checkEligibleOrder(){
    double btcUSDTPredictedVal, dogeBTCPredictedVal, ethBTCPredictedVal, dogeUSDTPredictedVal, ethUSDTPredictedVal;
    double btcUSDTavgPrice, dogeBTCavgPrice, ethBTCavgPrice, dogeUSDTavgPrice, ethUSDTavgPrice;

    std::vector<DataHolder> btcUSDTDataHolder = linearRegressionPrediction.btcUSDTDataHolder;
    std::vector<DataHolder> dogeBTCDataHolder = linearRegressionPrediction.dogeBTCDataHolder;
    std::vector<DataHolder> ethBTCDataHolder = linearRegressionPrediction.ethBTCDataHolder;
    std::vector<DataHolder> dogeUSDTDataHolder = linearRegressionPrediction.dogeUSDTDataHolder;
    std::vector<DataHolder> ethUSDTDataHolder = linearRegressionPrediction.ethUSDTDataHolder;  

    btcUSDTPredictedVal = linearRegressionPrediction.generatePredictions(btcUSDTDataHolder);
    dogeBTCPredictedVal = linearRegressionPrediction.generatePredictions(dogeBTCDataHolder);
    ethBTCPredictedVal = linearRegressionPrediction.generatePredictions(ethBTCDataHolder);
    dogeUSDTPredictedVal = linearRegressionPrediction.generatePredictions(dogeUSDTDataHolder);
    ethUSDTPredictedVal = linearRegressionPrediction.generatePredictions(ethUSDTDataHolder);    
    
    btcUSDTavgPrice = (btcUSDTDataHolder[btcUSDTDataHolder.size()-1].avgAsk + btcUSDTDataHolder[btcUSDTDataHolder.size()-1].avgBid) /2;
    dogeBTCavgPrice = (dogeBTCDataHolder[dogeBTCDataHolder.size()-1].avgAsk + dogeBTCDataHolder[dogeBTCDataHolder.size()-1].avgBid) /2;
    ethBTCavgPrice = (ethBTCDataHolder[ethBTCDataHolder.size()-1].avgAsk + ethBTCDataHolder[ethBTCDataHolder.size()-1].avgBid) /2;
    dogeUSDTavgPrice = (dogeUSDTDataHolder[dogeUSDTDataHolder.size()-1].avgAsk + dogeUSDTDataHolder[dogeUSDTDataHolder.size()-1].avgBid) /2;
    ethUSDTavgPrice = (ethUSDTDataHolder[ethUSDTDataHolder.size()-1].avgAsk + ethUSDTDataHolder[ethUSDTDataHolder.size()-1].avgBid) /2;
    

    // wallet always start with BTC only
    if(wallet.currencies["BTC"] > 0){

        //btcUSDT
        if (btcUSDTPredictedVal < btcUSDTavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "BTC/USDT", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "BTC/USDT", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateOfferWithPredictions("BTC/USDT",btcUSDTPredictedVal);
            }
        }
        
        // dogeBTC
        if (dogeBTCPredictedVal > dogeBTCavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "DOGE/BTC", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "DOGE/BTC", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateBidWithPredictions("DOGE/BTC",dogeBTCPredictedVal);
            }            
        }

        //ethBTC
        if (ethBTCPredictedVal > ethBTCavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "ETH/BTC", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "ETH/BTC", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateBidWithPredictions("ETH/BTC",ethBTCPredictedVal);
            }
        }
    }

    if(wallet.currencies["USDT"] > 0){

        //btcUSDT
        if (btcUSDTPredictedVal > btcUSDTavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "BTC/USDT", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "BTC/USDT", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateBidWithPredictions("BTC/USDT",btcUSDTPredictedVal);
            }            
        }

        //dogeUSDT
        if (dogeUSDTPredictedVal > dogeUSDTavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "DOGE/USDT", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "DOGE/USDT", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateBidWithPredictions("DOGE/USDT",dogeUSDTPredictedVal);
            } 
        }

        //ethUSDT
        if (ethUSDTPredictedVal > ethUSDTavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "ETH/USDT", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "ETH/USDT", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateBidWithPredictions("ETH/USDT",ethUSDTPredictedVal);
            }             
        }
    }

    if(wallet.currencies["ETH"] > 0){

        //ethUSDT
        if (ethUSDTPredictedVal < ethUSDTavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "ETH/USDT", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "ETH/USDT", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateOfferWithPredictions("ETH/USDT",ethUSDTPredictedVal);
            }                   
            
        }

        //ethBTC
        if (ethBTCPredictedVal < ethBTCavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "ETH/BTC", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "ETH/BTC", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateOfferWithPredictions("ETH/BTC",ethBTCPredictedVal);
            }
        }
    }

    if(wallet.currencies["DOGE"] > 0){

        //dogeUSDT
        if (dogeUSDTPredictedVal < dogeUSDTavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "DOGE/USDT", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "DOGE/USDT", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateOfferWithPredictions("DOGE/USDT",dogeUSDTPredictedVal);
            } 
        }

        //dogeBTC
        if (dogeBTCPredictedVal < dogeBTCavgPrice)
        {
            std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, "DOGE/BTC", currentTime );
            std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, "DOGE/BTC", currentTime );
            double lowestAskPrice = OrderBook::getLowPrice(askEntries);
            double highestBidPrice = OrderBook::getHighPrice(bidEntries);
            double spreadVal = ((lowestAskPrice - highestBidPrice)/lowestAskPrice) * 100;
            if(spreadVal < 0.02){
                generateOfferWithPredictions("DOGE/BTC",dogeBTCPredictedVal);
            } 
        }
    }
    
}

// When bidding usually want to take highest maximum bid.
// predicted value = next value.
void MerkelMain::generateBidWithPredictions(std::string productName, double predictedVal){
    double predictions, lowerThanPrediction, lowestPrice, bidAmount, walletAmount;

    std::vector<OrderBookEntry> entries;
        if(productName == "BTC/USDT"){
            entries = orderBook.getOrders(OrderBookType::ask, "BTC/USDT", currentTime );
        }

        if(productName == "DOGE/BTC"){
            entries = orderBook.getOrders(OrderBookType::ask, "DOGE/BTC", currentTime );
        }

        if(productName == "DOGE/USDT"){
            entries = orderBook.getOrders(OrderBookType::ask, "DOGE/USDT", currentTime );
        }

        if(productName == "ETH/BTC"){
            entries = orderBook.getOrders(OrderBookType::ask, "ETH/BTC", currentTime );
        }

        if(productName == "ETH/USDT"){
            entries = orderBook.getOrders(OrderBookType::ask, "ETH/USDT", currentTime );
        }

        for(OrderBookEntry entry : entries){
            //if entry Price is lower than prediction price, set it as that it is the lower than prediction
            if(entry.price < predictedVal) {
                lowerThanPrediction = entry.price;
                //after checking that entry price is lower than prediction, and theres a new entry price thats lower, set that as lowestPrice.
                if(entry.price <= lowerThanPrediction){
                    lowestPrice = entry.price;
                }
                if(entry.price == lowestPrice){
                    bidAmount = entry.amount;
                }
            }
        }

        OrderBookEntry obe {
                lowestPrice,
                bidAmount,
                currentTime,
                productName,
                OrderBookType::bid
        };
        obe.username = "simuser";

    if(wallet.canFulfillOrder(obe))
    {
        orderBook.insertOrder(obe);
        createAllSalesLogs(obe);
        std::cout <<"Bid has been made " <<std::endl;

    } else{
        std::vector<std::string> currs = CSVReader::tokenise(productName, '/');
        std::string currency = currs[1];
        for(std::pair<std::string,double> pair : wallet.currencies){
            std::cout<<"Wallet unable to handle with max Amount offered in order List, taking max amount you can order. " <<std::endl;
            walletAmount = pair.second;
            bidAmount = walletAmount/lowestPrice;

            OrderBookEntry obe {
                lowestPrice,
                bidAmount,
                currentTime,
                productName,
                OrderBookType::bid
            };
            if(wallet.canFulfillOrder(obe)){
                orderBook.insertOrder(obe);
                createAllSalesLogs(obe);
                std::cout <<"Bid has been made " <<std::endl;

            } else{
                std::cout<< "error becaue an order has already been made, new wallet value not updated. " << std::endl;
            }
        }  
    }
}

//remove the bid if a sale is not made. = matchAsksToBids fail

void MerkelMain::generateOfferWithPredictions(std::string productName, double predictedVal){
    double predictions, walletAmount;
    double currentPrice, askingAmount = 0;
    std::vector<OrderBookEntry> entries;

        if(productName == "BTC/USDT"){
            entries = orderBook.getOrders(OrderBookType::bid, "BTC/USDT", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);

        }

        if(productName == "DOGE/BTC"){
            entries = orderBook.getOrders(OrderBookType::bid, "DOGE/BTC", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);

        }

        if(productName == "DOGE/USDT"){
            entries = orderBook.getOrders(OrderBookType::bid, "DOGE/USDT", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);
        }

        if(productName == "ETH/BTC"){
            entries = orderBook.getOrders(OrderBookType::bid, "ETH/BTC", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);
        }

        if(productName == "ETH/USDT"){
            entries = orderBook.getOrders(OrderBookType::bid, "ETH/USDT", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);
        }

        for(OrderBookEntry entry : entries){
            //if entry Price is lower than prediction price, means value of product will go down.
            // example BTC/USDT will go down, USDT Value go up, BTC Value go down.
            if(predictedVal < entry.price) {
                currentPrice = entry.price*0.9999;
                askingAmount = entry.amount;
                break;
            }
        }

        if(askingAmount != 0 ){
            OrderBookEntry obe {
                currentPrice,
                askingAmount,
                currentTime,
                productName,
                OrderBookType::ask
            };
            obe.username = "simuser";

            if(wallet.canFulfillOrder(obe))
            {
                orderBook.insertOrder(obe);
                createAllSalesLogs(obe);
                std::cout <<"Ask has been made " <<std::endl;
            }else{
                std::vector<std::string> currs = CSVReader::tokenise(productName, '/');
                std::string currency = currs[0];
                for(std::pair<std::string,double> pair : wallet.currencies){
                    std::string currencyNameInWallet = pair.first;
                    std::cout<<"Wallet unable to handle current max Asking amount, taking max amount in wallet " <<std::endl;
                    walletAmount = pair.second;

                    OrderBookEntry obe {
                        currentPrice,
                        walletAmount,
                        currentTime,
                        productName,
                        OrderBookType::ask
                    };
                    
                    if(wallet.canFulfillOrder(obe)){
                        orderBook.insertOrder(obe);
                        createAllSalesLogs(obe);
                        std::cout <<"Ask has been made " <<std::endl;

                    } else{
                        std::cout<< "error becaue an order has already been made, new wallet value not updated. " << std::endl;
                    }
                }  
            }
        } else{
            std::cout << "No order to be made." << std::endl;
        }


}

void MerkelMain::ensureLogFilesEmpty(){
    logBot.open("AssetsLog.csv", std::ofstream::out | std::ofstream::trunc);
    logBot.close();

    logBot.open("AllSalesLog.csv", std::ofstream::out | std::ofstream::trunc);
    logBot.close();

    logBot.open("SuccessfulSalesLog.csv", std::ofstream::out | std::ofstream::trunc);
    logBot.close();
}

void MerkelMain::createAssetLogs(){
    //record assets for each timestamp
    logBot.open("AssetsLog.csv", std::ofstream::out | std::ofstream::app);
    logBot << "Time : " << currentTime << std::endl;
    logBot << "Assets : " << std::endl;
    logBot << wallet.toString() << std::endl;
    logBot.close();
}

void MerkelMain::createAllSalesLogs(OrderBookEntry obe){
    // record all bids and asks 
    logBot.open("AllSalesLog.csv", std::ofstream::out | std::ofstream::app);
    logBot << "Time : " << currentTime << std::endl;
    logBot << "Product Type : " << obe.orderBookTypeToString(obe.orderType) << std::endl;
    logBot << "Product Name : " << obe.product << std::endl;
    logBot << "Product Price : " << obe.price << std::endl;
    logBot << "Product Amount : " << obe.amount << std::endl; 
    logBot << " " << std::endl;
    logBot.close();
}

void MerkelMain::createSuccessfulSalesLogs(OrderBookEntry sale){
    std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, sale.product, currentTime );
    std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, sale.product, currentTime );
    double avgAsk = orderBook.getTotalPrice(askEntries) /askEntries.size();
    double avgBid = orderBook.getTotalPrice(bidEntries)/bidEntries.size();
  
    // record ONLY successful bids and asks 
    logBot.open("SuccessfulSalesLog.csv", std::ofstream::out | std::ofstream::app);
    logBot << "Time : " << currentTime << std::endl;
    logBot << "Product Type : " << sale.orderBookTypeToString(sale.orderType) << std::endl;
    logBot << "Product Name : " << sale.product << std::endl;
    logBot << "Product Price : " << sale.price << std::endl;
    logBot << "Product Amount : " << sale.amount << std::endl; 
    logBot << "Average Ask : " << avgAsk << std::endl;
    logBot << "Average Bid : " << avgBid << std::endl;
    logBot << " " << std::endl;
    logBot.close();

}

