#include "Logs.h"

Logs::Logs(){

}

void Logs::ensureLogFilesEmpty(){
    logBot.open("AssetsLog.csv", std::ofstream::out | std::ofstream::trunc);
    logBot.close();

    logBot.open("AllSalesLog.csv", std::ofstream::out | std::ofstream::trunc);
    logBot.close();

    logBot.open("SuccessfulSalesLog.csv", std::ofstream::out | std::ofstream::trunc);
    logBot.close();
}

void Logs::createAssetLogs(std::string currentTime , std::string currencyInWallet){
    //record assets for each timestamp
    logBot.open("AssetsLog.csv", std::ofstream::out | std::ofstream::app);
    logBot << "Time : " << currentTime << std::endl;
    logBot << "Assets : " << std::endl;
    logBot << currencyInWallet << std::endl;
    logBot.close();
}

void Logs::createAllSalesLogs(std::string currentTime, OrderBookEntry obe){
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