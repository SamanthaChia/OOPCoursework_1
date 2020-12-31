#pragma once

#include <vector>
#include <string>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"

class DataHolder{
    public:
        DataHolder(
            std::string _product,
            double _avgAsk,
            int askVol,
            double _avgBid,
            int bidVol
        );
};