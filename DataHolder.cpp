#include "DataHolder.h"

DataHolder::DataHolder(
    std::string _product,
    double _avgAsk,
    int _askVol,
    double _avgBid,
    int _bidVol
    )
: product(_product),
  avgAsk(_avgAsk),
  askVol(_askVol),
  avgBid(_avgBid),
  bidVol(_bidVol)
{

}

double DataHolder::getAvgAsk(std::vector<DataHolder>& data){
    return data[0].avgAsk;
}

double DataHolder::getAvgBid(std::vector<DataHolder>& data){
    return data[0].avgBid;
}

int DataHolder::getAskVol(std::vector<DataHolder>& data){
    return data[0].askVol;
}

int DataHolder::getBidVol(std::vector<DataHolder>& data){
    return data[0].bidVol;
}


    