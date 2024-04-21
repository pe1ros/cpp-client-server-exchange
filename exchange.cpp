#include "exchange.h"

void Exchange::AddOrder(const Order& order) {
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

// Продажа должна быть выше покупки
bool compareOrders(const Order& order1, const Order& order2) {
    if (order1.type != order2.type) {
        return order1.type == Type::Sell;
    }
    return order1.price > order2.price;
}

void Exchange::ProcessOrders() {
    // Сортируем заявки по цене
    std::sort(orders_.begin(), orders_.end(), compareOrders);

    for (auto& order : orders_) {
        std::cout << order << '\n';
        if (order.type == Type::Buy) {
            // Ищем первую заявку на продажу с ценой не выше текущей покупки
            auto sell_order = std::find_if(orders_.begin(), orders_.end(), [&](const Order& o) {
                return o.type == Type::Sell && o.price <= order.price && o.volume > 0;
            });

            // Если такая заявка найдена, заключаем сделку
            if (sell_order != orders_.end()) {
                int trade_volume = std::min(order.volume, sell_order->volume);
                std::cout << "Trade: " << trade_volume << " at price " << sell_order->price << std::endl;

                // Обновляем объемы заявок и балансы пользователей
                order.volume -= trade_volume;
                sell_order->volume -= trade_volume;

                if (sell_order->volume > 0) {
                    std::cout << "Remaining sell order: " << sell_order->volume << " at price " << sell_order->price << std::endl;
                }
            }
        }
    }
}