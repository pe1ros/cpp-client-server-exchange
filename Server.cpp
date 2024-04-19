#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "json.hpp"
#include "Common.hpp"
#include "Exchange.hpp"

using boost::asio::ip::tcp;

class Core
{
public:
    // "Регистрирует" нового пользователя и возвращает его ID.
    std::string RegisterNewUser(const std::string& aUserName)
    {
        size_t newUserId = mUsers.size();
        mUsers[newUserId] = aUserName;

        return std::to_string(newUserId);
    }

    // Запрос имени клиента по ID
    std::string GetUserName(const std::string& aUserId)
    {
        const auto userIt = mUsers.find(std::stoi(aUserId));
        if (userIt == mUsers.cend())
        {
            return "Error! Unknown User";
        }
        else
        {
            return userIt->second;
        }
    }

    void ProcessTradingRequest(const nlohmann::json& request)
    {
        Order order;
        order.volume = request["Volume"];
        order.price = request["Price"];
        order.userId = request["UserId"];
        order.type = request["Type"] == "Buy" ? Type::Buy : Type::Sell;

        exchange_.AddOrder(order);
    }

    std::string GetUserBalance(const std::string& userId)
    {
        return exchange_.GetBalance(userId);
    }

private:
    // <UserId, UserName>
    std::map<size_t, std::string> mUsers;
    Exchange exchange_;
};

Core& GetCore()
{
    static Core core;
    return core;
}

class session
{
public:
    session(boost::asio::io_service& io_service)
        : socket_(io_service)
    {
    }

    tcp::socket& socket()
    {
        return socket_;
    }

    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }

    // Обработка полученного сообщения.
    void handle_read(const boost::system::error_code& error,
        size_t bytes_transferred)
    {
        if (!error) {
            data_[bytes_transferred] = '\0';
            auto j = nlohmann::json::parse(data_);

            // Обработка запроса в зависимости от типа запроса
            auto reqType = j["ReqType"];
            std::string reply;
            if (reqType == Requests::Registration) {
                reply = GetCore().RegisterNewUser(j["Message"]);
            } else if (reqType == Requests::Hello) {
                reply = "Hello, " + GetCore().GetUserName(j["UserId"]) + "!\n";
            } else if (reqType == Requests::Trading) {
                GetCore().ProcessTradingRequest(j);
                reply = "Order added\n";
            } else if (reqType == Requests::Balance) {
                reply = GetCore().GetUserBalance(j["UserId"]);
            } else  {
                reply = "Error! Unknown request type";
            }

            boost::asio::async_write(socket_,
                boost::asio::buffer(reply, reply.size()),
                boost::bind(&session::handle_write, this,
                    boost::asio::placeholders::error));
        } else {
            delete this;
        }
    }

    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                boost::bind(&session::handle_read, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }
    }

private:
    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class server
{
public:
    server(boost::asio::io_service& io_service)
        : io_service_(io_service),
        acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
    {
        std::cout << "Server started! Listen " << port << " port" << std::endl;

        session* new_session = new session(io_service_);
        acceptor_.async_accept(new_session->socket(),
            boost::bind(&server::handle_accept, this, new_session,
                boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
        const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
            new_session = new session(io_service_);
            acceptor_.async_accept(new_session->socket(),
                boost::bind(&server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
        }
        else
        {
            delete new_session;
        }
    }

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        static Core core;

        server s(io_service);

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}