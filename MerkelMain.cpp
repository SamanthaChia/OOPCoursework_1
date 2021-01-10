#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "DataHolder.h"
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
    logs.ensureLogFilesEmpty();
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
    double costAfterMatching;
    std::cout << "Continue to next time step." << std::endl;
    for(std::string p : orderBook.getKnownProducts())
    {
        std::cout << "Matching : " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sale number : " << sales.size() << std::endl;
        for(OrderBookEntry& sale : sales)
        {   
            if(sale.username == "simuser")
            {
                wallet.processSale(sale);
                costAfterMatching = sale.price * sale.amount;
                std::cout << sale.product << " with the price : " << sale.price << ", amount : " << sale.amount << " cost : " << costAfterMatching << " was successful! " << std::endl;
                logs.createSuccessfulSalesLogs(currentTime, sale, orderBook);
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
        while(true){        
            automateGenerateDataHolder();
            logs.createAssetLogs(currentTime, wallet.toString());
            checkEligibleOrder();
            printWallet();
            previousTimeFrame = currentTime;
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

void MerkelMain::automateGenerateDataHolder(){
    int index = 0;
    //run for 10 times to obtain DataHolders.
    for(int i =0; i<10;){
        linearRegressionPrediction.generateDataHolder(currentTime, orderBook);
        currentTime = orderBook.getNextTime(currentTime);
        i++;
    }

    //before processing, remove previous simuser stuff
    for(std::string const& p : orderBook.getKnownProducts()){
        std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, p, previousTimeFrame );
        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, p, previousTimeFrame );
        //go through bid first
        for(OrderBookEntry obe : bidEntries){
            if(obe.username == "simuser"){
                bidEntries.erase(bidEntries.begin() + index);
                std::cout << obe.orderBookTypeToString(obe.orderType) << " for " << obe.product << " has been withdrawn from orderBook " << std::endl;
            } else
            {
                index++;
            }
        }

        //go through askEntries
        for(OrderBookEntry obe: askEntries){
            if(obe.username == "simuser"){
                askEntries.erase(askEntries.begin() + index);
                std::cout << obe.orderBookTypeToString(obe.orderType) << " for " << obe.product << " has been withdrawn from orderBook " << std::endl;

            }else
            {
                index++;
            }
        }
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
    
    entries = orderBook.getOrders(OrderBookType::ask, productName, currentTime );

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
        logs.createAllSalesLogs(currentTime, obe);
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
                logs.createAllSalesLogs(currentTime, obe);
                std::cout <<"Bid has been made " <<std::endl;

            } else{
                std::cout<< "error becaue an order has already been made, new wallet value not updated. " << std::endl;
            }
        }  
    }
}

void MerkelMain::generateOfferWithPredictions(std::string productName, double predictedVal){
    double predictions, walletAmount;
    double currentPrice, askingAmount = 0;
    std::vector<OrderBookEntry> entries;

    entries = orderBook.getOrders(OrderBookType::bid, productName, currentTime );
    std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);

    for(OrderBookEntry entry : entries){
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
            logs.createAllSalesLogs(currentTime, obe);
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
                    logs.createAllSalesLogs(currentTime, obe);
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