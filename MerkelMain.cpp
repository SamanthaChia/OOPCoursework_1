#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "DataHolder.h"
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>

MerkelMain::MerkelMain(){

}

void MerkelMain::init(){
    
    int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);
    automatePredictionBot();
    while(true){
        printMenu();
        input = getUserOption();
        procesUserOption(input);
    }
}

void MerkelMain::printMenu(){

    // 1 print help
    std::cout << "1: Print help. " << std::endl;
    // 2 print exchange stats
    std::cout << "2 : Print exchange stats. " << std::endl;
    // 3 make an offer
    std::cout << "3 : Make an offer. " << std::endl;
    // 4 make a bid
    std::cout << "4 : Make a bid. " << std::endl;
    // 5 printi wallet
    std::cout << "5 : Show wallet. " << std::endl;
    // 6 continue
    std::cout << "6 : Continue. " << std::endl;

    std::cout << "================ " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Please enter a value from 1 - 6" << std::endl;
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
    // std::cout << "Make an ask - enter the amount: product, price, amount, eg ETH/BTC, 200,0.5 " << std::endl;
    // std::string input;
    // std::getline(std::cin, input);

    // std::vector<std::string> tokens = CSVReader::tokenise(input,',');
    // if(tokens.size() != 3){
    //     std::cout << "MerkelMain::enterAsk Bad input " << std::endl;
    // }

    // else{
    //     try{
    //         OrderBookEntry obe = CSVReader::stringsToOBE(
    //             tokens[1],
    //             tokens[2],
    //             currentTime,
    //             tokens[0],
    //             OrderBookType::ask
    //         );
    //         obe.username = "simuser";

    //         if(wallet.canFulfillOrder(obe))
    //         {
    //             std::cout<<"Wallet looks good. " << std::endl;
    //             orderBook.insertOrder(obe);
    //         }
    //         else{
    //             std::cout<< "Wallet has insufficient funds. " << std::endl;
    //         }
    //     }catch(const std::exception& e)
    //     {
    //         std::cout << "MerkelMain::enterAsk Bad input " << std::endl;
    //     }
    // }
    // generateBidWithPredictions("ETH/BTC");
    checkEligibleOrder();
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
    // generateBidWithPredictions("BTC/USDT");
    // generateBidWithPredictions("DOGE/BTC");
    // generateBidWithPredictions("DOGE/USDT");
    // generateBidWithPredictions("ETH/BTC");
    // generateBidWithPredictions("ETH/USDT");

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
            }
        }
    }
    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelMain::procesUserOption(int userOption){
    if(userOption <= 0 || userOption > 6){
        std::cout << "You have entered an invalid option." << std::endl;
    }
    else if(userOption == 1){
        printHelp();
    }
    else if(userOption == 2){
        printMarketStats();
    }
    else if(userOption == 3){
        enterAsk();
    }
    else if(userOption == 4){
        enterBid();
    }
    else if(userOption == 5){
        printWallet();
    }
    else if(userOption == 6){
        gotoNextTimeFrame();
    }
}

void MerkelMain::generateDataHolder(){
    std::vector<DataHolder> dataHolderBook;
    double askVol, bidVol;
    for(std::string const& p : orderBook.getKnownProducts()){
        askVol = 0;
        bidVol = 0;
        std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, p, currentTime );
        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, p, currentTime );

        for(OrderBookEntry askEntry : askEntries){
            askVol += askEntry.amount;
        }
        double avgAsk = orderBook.getTotalPrice(askEntries) /askEntries.size();
        
        for(OrderBookEntry bidEntry : bidEntries){
            bidVol += bidEntry.amount;
        }
        double avgBid = orderBook.getTotalPrice(bidEntries)/bidEntries.size();

        DataHolder dh {
            p,
            avgAsk,
            askVol,
            avgBid,
            bidVol
        };

		dataHolderBook.push_back(dh);

        if(p == "BTC/USDT"){
            btcUSDTDataHolder.push_back(dh);
        }

        if(p == "DOGE/BTC"){
            dogeBTCDataHolder.push_back(dh);    
        }

        if(p == "DOGE/USDT"){
            dogeUSDTDataHolder.push_back(dh);     
        }

        if(p == "ETH/BTC"){
            ethBTCDataHolder.push_back(dh);
        }

        if(p == "ETH/USDT"){
            ethUSDTDataHolder.push_back(dh);
        }

    }
}

void MerkelMain::automatePredictionBot(){
    //after 10 times
    for(int i =0; i<10;){
        generateDataHolder();
        currentTime = orderBook.getNextTime(currentTime);
        i++;
    }
    for(std::string const& p : orderBook.getKnownProducts()){
        if(p == "BTC/USDT"){
            generatePredictions(btcUSDTDataHolder);
        }

        if(p == "DOGE/BTC"){
            generatePredictions(dogeBTCDataHolder);
        }

        if(p == "DOGE/USDT"){
            generatePredictions(dogeUSDTDataHolder);
        }

        if(p == "ETH/BTC"){
            generatePredictions(ethBTCDataHolder);
        }

        if(p == "ETH/USDT"){
            generatePredictions(ethUSDTDataHolder);
        }
    }
}

//used to be void changed to double to return predictedValue
double MerkelMain::generatePredictions(std::vector<DataHolder> productData){
    std::vector<double> x,y;
    std::vector<PredictB0B1> errorVal;
    double predictedValue, error, b1, b0, currentPrice, askBidRatio, avgGrowthRatio;
    double learningVal = 0.0001;


    for(int i=0;i<productData.size()-1;++i)
    {
        askBidRatio = productData[i].askVol / productData[i].bidVol;
        avgGrowthRatio = ((productData[i].avgAsk - productData[i + 1].avgAsk) + (productData[i].avgBid - productData[i + 1].avgBid)) / 2;
        // x = ratio between askVol and bidVol
        // y = avg growth/loss ratio
        x.push_back(askBidRatio);
        std::cout << "askBidRatio : " << askBidRatio << std::endl;
        y.push_back(avgGrowthRatio);
        std::cout << "avgGrowthRatio : " << avgGrowthRatio << std::endl;
        
    }


        std::cout << "Product: " << productData[0].product << std::endl;
        if(currentTime == orderBook.getEarliestTime()){
            b1 = 0;
            b0 = 0;
        }

        for(int i = 0; i < x.size() * 10; i++)
        {
            int idx = i % x.size();
            predictedValue = b1 * x[idx] + b0;
            error = predictedValue - y[idx];
            b0 = b0 - learningVal * error;
            b1 = b1 - learningVal * error * x[idx]; 

            PredictB0B1 predictB0B1 {
                error,
                b0,
                b1
            };

            errorVal.push_back(predictB0B1);
        }

        std::sort(errorVal.begin(),errorVal.end(),[](const PredictB0B1& lhs, const PredictB0B1& rhs){
            return abs(lhs.error) < abs(rhs.error);
        });

        std::cout << "After sorting = b0: " << errorVal[0].b0 << " b1 : " << errorVal[0].b1 << " error : " << errorVal[0].error << std::endl;
        
        
        std::cout << "x val : " << x[x.size()-1] <<std::endl;
        currentPrice = (productData[productData.size()-1].avgAsk + productData[productData.size()-1].avgBid)/2;
        std::cout << "currentPrice Value : " << currentPrice << std::endl;
        
        // x[x.size()-1] because latest x value,
        // + currentPrice again because it will  only be % of the currentPrice
        // predictedVal = newB0 + newB1 * x[x.size()-1] * currentPrice + currentPrice 
        predictedValue= (errorVal[0].b0+ errorVal[0].b1 * x[x.size()-1] ) * currentPrice + currentPrice ;

        
        std::cout << "Predicted Value : " << predictedValue << std::endl;
        std::cout << " " <<std::endl;

        return predictedValue;
}

void MerkelMain::checkEligibleOrder(){
    double btcUSDTPredictedVal, dogeBTCPredictedVal, ethBTCPredictedVal, dogeUSDTPredictedVal, ethUSDTPredictedVal;
    double btcUSDTavgPrice, dogeBTCavgPrice, ethBTCavgPrice, dogeUSDTavgPrice, ethUSDTavgPrice;

    btcUSDTPredictedVal = generatePredictions(btcUSDTDataHolder);
    dogeBTCPredictedVal = generatePredictions(dogeBTCDataHolder);
    ethBTCPredictedVal = generatePredictions(ethBTCDataHolder);
    dogeUSDTPredictedVal = generatePredictions(dogeUSDTDataHolder);
    ethUSDTPredictedVal = generatePredictions(ethUSDTDataHolder);    
    
    btcUSDTavgPrice = (btcUSDTDataHolder[btcUSDTDataHolder.size()-1].avgAsk + btcUSDTDataHolder[btcUSDTDataHolder.size()-1].avgBid) /2;
    dogeBTCavgPrice = (dogeBTCDataHolder[dogeBTCDataHolder.size()-1].avgAsk + dogeBTCDataHolder[dogeBTCDataHolder.size()-1].avgBid) /2;
    ethBTCavgPrice = (ethBTCDataHolder[ethBTCDataHolder.size()-1].avgAsk + ethBTCDataHolder[ethBTCDataHolder.size()-1].avgBid) /2;
    dogeUSDTavgPrice = (dogeUSDTDataHolder[dogeUSDTDataHolder.size()-1].avgAsk + dogeUSDTDataHolder[dogeUSDTDataHolder.size()-1].avgBid) /2;
    ethUSDTavgPrice = (ethUSDTDataHolder[ethUSDTDataHolder.size()-1].avgAsk + ethUSDTDataHolder[ethUSDTDataHolder.size()-1].avgBid) /2;
    
    // wallet always start with BTC only
    if(wallet.currencies["BTC"] > 0){
        //btcUSDT
        if (btcUSDTPredictedVal > btcUSDTavgPrice)
        {
            generateBidWithPredictions("BTC/USDT",btcUSDTPredictedVal);
        } else{
            generateOfferWithPredictions("BTC/USDT",btcUSDTPredictedVal);
        }
        
        //dogeBTC
        if (dogeBTCPredictedVal > dogeBTCavgPrice)
        {
            generateBidWithPredictions("DOGE/BTC",dogeBTCPredictedVal);
        } else{
            generateOfferWithPredictions("DOGE/BTC",dogeBTCPredictedVal);
        }

        //ethBTC
        if (ethBTCPredictedVal > ethBTCavgPrice)
        {
            generateBidWithPredictions("ETH/BTC",ethBTCPredictedVal);
        } else{
            generateOfferWithPredictions("ETH/BTC",ethBTCPredictedVal);
        }
    }

    if(wallet.currencies["USDT"] > 0){
        if (btcUSDTPredictedVal > btcUSDTavgPrice)
        {
            generateBidWithPredictions("BTC/USDT",btcUSDTPredictedVal);
        } else{
            generateOfferWithPredictions("BTC/USDT",btcUSDTPredictedVal);
        }

        if (dogeUSDTPredictedVal > dogeUSDTavgPrice)
        {
            generateBidWithPredictions("DOGE/USDT",dogeUSDTPredictedVal);
        } else{
            generateOfferWithPredictions("DOGE/USDT",dogeUSDTPredictedVal);
        }

        if (ethUSDTPredictedVal > ethUSDTavgPrice)
        {
            generateBidWithPredictions("ETH/USDT",ethUSDTPredictedVal);
        } else{
            generateOfferWithPredictions("ETH/USDT",ethUSDTPredictedVal);
        }
    }

    if(wallet.currencies["ETH"] > 0){
        if (ethUSDTPredictedVal > ethUSDTavgPrice)
        {
            generateBidWithPredictions("ETH/USDT",ethUSDTPredictedVal);
        } else{
            generateOfferWithPredictions("ETH/USDT",ethUSDTPredictedVal);
        }

        if (ethBTCPredictedVal > ethBTCavgPrice)
        {
            generateBidWithPredictions("ETH/BTC",ethBTCPredictedVal);
        } else{
            generateOfferWithPredictions("ETH/BTC",ethBTCPredictedVal);
        }
    }

    if(wallet.currencies["DOGE"] > 0){
        if (dogeUSDTPredictedVal > dogeUSDTavgPrice)
        {
            generateBidWithPredictions("DOGE/USDT",dogeUSDTPredictedVal);
        } else{
            generateOfferWithPredictions("DOGE/USDT",dogeUSDTPredictedVal);
        }

        //dogeBTC
        if (dogeBTCPredictedVal > dogeBTCavgPrice)
        {
            generateBidWithPredictions("DOGE/BTC",dogeBTCPredictedVal);
        } else{
            generateOfferWithPredictions("DOGE/BTC",dogeBTCPredictedVal);
        }
    }
    
}

// When bidding usually want to take highest maximum bid.
// predicted value = next value.
void MerkelMain::generateBidWithPredictions(std::string productName, double predictedVal){
    double predictions, lowerThanPrediction, lowestPrice, bidAmount, walletAmount;

    std::vector<OrderBookEntry> entries;
        if(productName == "BTC/USDT"){
            // predictions = generatePredictions(btcUSDTDataHolder);
            entries = orderBook.getOrders(OrderBookType::ask, "BTC/USDT", currentTime );
        }

        if(productName == "DOGE/BTC"){
            // predictions = generatePredictions(dogeBTCDataHolder);
            entries = orderBook.getOrders(OrderBookType::ask, "DOGE/BTC", currentTime );
        }

        if(productName == "DOGE/USDT"){
            // predictions = generatePredictions(dogeUSDTDataHolder);
            entries = orderBook.getOrders(OrderBookType::ask, "DOGE/USDT", currentTime );
        }

        if(productName == "ETH/BTC"){
            // predictions = generatePredictions(ethBTCDataHolder);
            entries = orderBook.getOrders(OrderBookType::ask, "ETH/BTC", currentTime );
        }

        if(productName == "ETH/USDT"){
            // predictions = generatePredictions(ethUSDTDataHolder);
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
        std::cout <<"Bid has been made " <<std::endl;

    } else{
        std::vector<std::string> currs = CSVReader::tokenise(productName, '/');
        std::string currency = currs[1];
        for(std::pair<std::string,double> pair : wallet.currencies){
            std::string currencyNameInWallet = pair.first;
            if(currencyNameInWallet != currency){
                std::cout<< "Wallet does not have this currency. " << std::endl;
                continue;
            }
            else if(currencyNameInWallet == currency)
            {
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
                    std::cout <<"Bid has been made " <<std::endl;

                } else{
                    std::cout<< "error somewhere " << std::endl;
                }
            }
        }  
    }
    
    // //matching
    // std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(productName, currentTime);
    // for(OrderBookEntry& sale : sales)
    // {   
    //     if(sale.username == "simuser")
    //     {
    //     }
    // }

    // currentTime = orderBook.getNextTime(currentTime);
    
}

//remove the bid if a sale is not made. = matchAsksToBids fail

void MerkelMain::generateOfferWithPredictions(std::string productName, double predictedVal){
    double predictions, walletAmount;
    double currentPrice, askingAmount = 0;
    std::vector<OrderBookEntry> entries;

        if(productName == "BTC/USDT"){
            // predictions = generatePredictions(btcUSDTDataHolder);
            entries = orderBook.getOrders(OrderBookType::bid, "BTC/USDT", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);

        }

        if(productName == "DOGE/BTC"){
            // predictions = generatePredictions(dogeBTCDataHolder);
            entries = orderBook.getOrders(OrderBookType::bid, "DOGE/BTC", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);

        }

        if(productName == "DOGE/USDT"){
            // predictions = generatePredictions(dogeUSDTDataHolder);
            entries = orderBook.getOrders(OrderBookType::bid, "DOGE/USDT", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);

        }

        if(productName == "ETH/BTC"){
            // predictions = generatePredictions(ethBTCDataHolder);
            entries = orderBook.getOrders(OrderBookType::bid, "ETH/BTC", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);

        }

        if(productName == "ETH/USDT"){
            // predictions = generatePredictions(ethUSDTDataHolder);
            entries = orderBook.getOrders(OrderBookType::bid, "ETH/USDT", currentTime );
            std::sort(entries.begin(), entries.end(), OrderBookEntry::compareByPriceDesc);
        }

        for(OrderBookEntry entry : entries){
            //if entry Price is lower than prediction price, means value of product will go down.
            // example BTC/USDT will go down, USDT Value go up, BTC Value go down.
            if(predictedVal < entry.price) {
                currentPrice = entry.price;
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
                std::cout <<"Ask has been made " <<std::endl;
            }else{
                std::vector<std::string> currs = CSVReader::tokenise(productName, '/');
                std::string currency = currs[0];
                std::cout<< "currency : " << currency << std::endl;
                for(std::pair<std::string,double> pair : wallet.currencies){
                    std::string currencyNameInWallet = pair.first;
                    std::cout << "currencyNameInWallet : " << currencyNameInWallet << std::endl; 
                    if(currencyNameInWallet != currency){
                        std::cout<< "Wallet does not have this currency. " << std::endl;
                        continue;
                    }
                    else if(currencyNameInWallet == currency)
                    {
                        std::cout<<"Wallet unable to current max Asking amount, recalculating asking amount " <<std::endl;
                        walletAmount = pair.second;

                        OrderBookEntry obe {
                            currentPrice,
                            walletAmount,
                            currentTime,
                            productName,
                            OrderBookType::ask
                        };
                        std::cout <<"CurrentPrice : " << currentPrice <<std::endl;
                        std::cout <<"askingAmount : " << askingAmount <<std::endl;
                        std::cout <<"currentTime : " << currentTime <<std::endl;
                        std::cout <<"productName : " << productName <<std::endl;
                        
                        if(wallet.canFulfillOrder(obe)){
                            orderBook.insertOrder(obe);
                            std::cout <<"Ask has been made " <<std::endl;

                        } else{
                            std::cout<< "error somewhere " << std::endl;
                        }
                    }
                }  
            }
        } else{
            std::cout << "No order to be made." << std::endl;
        }


}

