#include <string>
#include <map>
#include "OrderBookEntry.h"

class Wallet
{
    public:
        Wallet();
        /** insert currency into the wallet */
        void insertCurrency(std::string type, double amount);
        /** remove currency from the wallet */
        bool removeCurrency(std::string type, double amount);

        /** check if the wallet contains this much currency or more */
        bool containsCurrency(std::string type, double amount);

        /** check if the wallet can fulfill the ask or bid */
        bool canFulfillOrder(OrderBookEntry order);
        
        /** update the contents of the wallet
         * asumes the order was made by the owner of the wallet
         */
        void processSale(OrderBookEntry& sale);
        
        /** generate a string representation of the wallet */
        std::string toString();

    private:
        std::map<std::string,double> currencies;


};