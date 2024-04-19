#pragma once

#include <algorithm>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

enum class Type {
    Buy,
    Sell
};

struct Order {
    int volume;
    int price;
    Type type;
    std::string user_id;
};

inline std::ostream& operator<<(std::ostream& oss, const Order& order) {
    oss << "Volume: " << order.volume << "\n"
        << "Price: " << order.price << "\n"
        << "Type: " << (order.type == Type::Buy? "Buy" : "Sell") << "\n"
        << "User id: " << order.user_id << "\n";
    return oss;
}

class Exchange {
public:
    void AddOrder(const Order& order);

    std::string GetBalance(const std::string& user_id);

private:
    std::vector<Order> orders_;

    void ProcessOrders();
};
