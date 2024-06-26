#include <cstdlib>
#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "json.hpp"
#include "common.h"
#include "exchange.h"

using boost::asio::ip::tcp;

class Core
{
public:
    std::string RegisterNewUser(const std::string& user_name)
{
        size_t new_user_id = users_.size();
        users_[new_user_id] = user_name;

        return std::to_string(new_user_id);
    }

    std::string GetUserNameById(const std::string& aUserId)
    {
        const auto userIt = users_.find(std::stoi(aUserId));
        if (userIt == users_.cend())
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
        order.user_id = request["UserId"];
        order.type = request["Type"] == "Buy" ? Type::Buy : Type::Sell;

        exchange_.AddOrder(order);
    }

    std::string GetUserBalance(const std::string& user_id)
    {
        return exchange_.GetBalance(user_id);
    }

private:
    // <UserId, UserName>
    std::map<size_t, std::string> users_;
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
            auto req_type = j["ReqType"];
            std::string reply;
            if (req_type == Requests::Registration) {
                reply = GetCore().RegisterNewUser(j["Message"]);
            } else if (req_type == Requests::Hello) {
                reply = "Hello, " + GetCore().GetUserNameById(j["UserId"]) + "!\n";
            } else if (req_type == Requests::Trading) {
                GetCore().ProcessTradingRequest(j);
                reply = "Order added\n";
            } else if (req_type == Requests::Balance) {
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