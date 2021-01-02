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


    