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

void Logs::createAssetLogs(std::string currentTime , std::string wallet){
    //record assets for each timestamp
    logBot.open("AssetsLog.csv", std::ofstream::out | std::ofstream::app);
    logBot << "Time : " << currentTime << std::endl;
    logBot << "Assets : " << std::endl;
    logBot << wallet << std::endl;
    logBot.close();
}