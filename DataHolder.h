#pragma once

#include <vector>
#include <string>

class DataHolder{
    public:
        DataHolder(
            std::string _product,
            double _avgAsk,
            double askVol,
            double _avgBid,
            double bidVol
        );

        std::string product;
        double avgAsk;
        double avgBid;
        double askVol;
        double bidVol;

};