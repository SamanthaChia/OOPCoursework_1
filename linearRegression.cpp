#include "LinearRegression.h"
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "PredictB0B1.h"
#include <algorithm>

LinearRegression::LinearRegression(){

}

void LinearRegression::generateDataHolder(std::string currentTime, OrderBook orderBook){
    double askVol, bidVol;
    for(std::string const& p : orderBook.getKnownProducts()){
        std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask, p, currentTime );
        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid, p, currentTime );
        askVol = 0;
        bidVol = 0;

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

double LinearRegression::generatePredictions(std::vector<DataHolder> productData){
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
        y.push_back(avgGrowthRatio);
        
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

    currentPrice = (productData[productData.size()-1].avgAsk + productData[productData.size()-1].avgBid)/2;
    
    // x[x.size()-1] because latest x value,
    // + currentPrice again because it will  only be % of the currentPrice
    // predictedVal = newB0 + newB1 * x[x.size()-1] * currentPrice + currentPrice 
    predictedValue= (errorVal[0].b0+ errorVal[0].b1 * x[x.size()-1] ) * currentPrice + currentPrice ;

    return predictedValue;
}