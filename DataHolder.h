#pragma once

#include <vector>
#include <string>

class DataHolder{
    public:
        DataHolder(
            std::string _product,
            double _avgAsk,
            int askVol,
            double _avgBid,
            int bidVol
        );

        std::string product;
        double avgAsk;
        double avgBid;
        int askVol;
        int bidVol;

};