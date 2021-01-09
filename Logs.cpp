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