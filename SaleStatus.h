#pragma once

#include "OrderBookEntry.h"

class SaleStatus{
    public:
        SaleStatus(
            OrderBookEntry _obe,
            bool _saleStatus
        );

        OrderBookEntry obe;
        bool saleStatus;
};