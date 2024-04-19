#include "exchange.h"

void Exchange::AddOrder(const Order& order) {
    std::cout << order;
    orders_.push_back(order);
    ProcessOrders();
}

std::string Exchange::GetBalance(const std::string& user_id) {
    int balance_USD = 0;
    int balance_RUB = 0;

    for (const auto& order : orders_) {
        if (order.user_id == user_id) {
            if (order.type == Type::Buy) {
                balance_USD -= order.volume;
                balance_RUB += order.volume * order.price;
            } else {
                balance_USD += order.volume;
                balance_RUB -= order.volume * order.price;
            }
        }
    }

    std::ostringstream oss;
    oss << "User balance: " << balance_USD << " USD, " << balance_RUB << " RUB." << "\n";
    return oss.str();
}

void Exchange::ProcessOrders() {}