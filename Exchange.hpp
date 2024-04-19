
#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

enum class Type
{
    Buy,
    Sell
};

struct Order
{
    int volume;
    int price;
    Type type;
    std::string userId;
};

std::ostream& operator<<(std::ostream& oss, const Order& order) {
    oss << "Volume: " << order.volume << "\n"
        << "Price: " << order.price << "\n"
        << "Type: " << (order.type == Type::Buy? "Buy" : "Sell") << "\n"
        << "User id: " << order.userId << "\n";
    return oss;
}

class Exchange
{
public:
    // Добавляем заявку в биржу
    void AddOrder(const Order& order)
    {
        std::cout << order;
        orders.push_back(order);
        ProcessOrders();
    }

    std::string GetBalance(const std::string& userId) {
        int balanceUSD = 0;
        int balanceRUB = 0;
        for (const auto& order : orders) {
            if (order.userId == userId) {
                if (order.type == Type::Buy) {
                    balanceUSD -= order.volume;
                    balanceRUB += order.volume * order.price;
                } else {
                    balanceUSD += order.volume;
                    balanceRUB -= order.volume * order.price;
                }
            }
        }
        std::ostringstream oss;
        oss << "User balance: " << balanceUSD << " USD, " << balanceRUB << " RUB." << "\n";
        return oss.str();
    }


private:
    std::vector<Order> orders;

    void ProcessOrders() {}
};
