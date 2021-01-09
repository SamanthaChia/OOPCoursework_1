#include "linearRegression.h"
#include "OrderBookEntry.h"

linearRegression::linearRegression(){

}

void linearRegression::generateDataHolder(std::string currentTime){
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