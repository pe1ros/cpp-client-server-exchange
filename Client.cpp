#include <iostream>
#include <boost/asio.hpp>
#include <string>

#include "common.h"
#include "json.hpp"

using boost::asio::ip::tcp;

void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const std::string& aMessage)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Message"] = aMessage;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

void SendMessage(
    tcp::socket& aSocket,
    const std::string& aId,
    const std::string& aRequestType,
    const int& volume,
    const int& price,
    const std::string type)
{
    nlohmann::json req;
    req["UserId"] = aId;
    req["ReqType"] = aRequestType;
    req["Volume"] = volume;
    req["Price"] = price;
    req["Type"] = type;

    std::string request = req.dump();
    boost::asio::write(aSocket, boost::asio::buffer(request, request.size()));
}

// Возвращает строку с ответом сервера на последний запрос.
std::string ReadMessage(tcp::socket& aSocket)
{
    boost::asio::streambuf b;
    boost::asio::read_until(aSocket, b, "\0");
    std::istream is(&b);
    std::string line(std::istreambuf_iterator<char>(is), {});
    return line;
}

// "Создаём" пользователя, получаем его ID.
std::string ProcessRegistration(tcp::socket& aSocket)
{
    std::string name;
    std::cout << "Hello! Enter your name: ";
    std::cin >> name;

    // Для регистрации Id не нужен, заполним его нулём
    SendMessage(aSocket, "0", Requests::Registration, name);
    return ReadMessage(aSocket);
}

int main()
{
    try
    {
        boost::asio::io_service io_service;

        tcp::resolver resolver(io_service);
        tcp::resolver::query query(tcp::v4(), "127.0.0.1", std::to_string(port));
        tcp::resolver::iterator iterator = resolver.resolve(query);

        tcp::socket s(io_service);
        s.connect(*iterator);

        // Мы предполагаем, что для идентификации пользователя будет использоваться ID.
        // Тут мы "регистрируем" пользователя - отправляем на сервер имя, а сервер возвращает нам ID.
        // Этот ID далее используется при отправке запросов.
        std::string my_id = ProcessRegistration(s);

        while (true)
        {
            // Тут реализовано "бесконечное" меню.
            std::cout << "Menu:\n"
                         "1) Hello Request\n"
                         "2) Trade\n"
                         "3) Balance\n"
                         "4) Exit\n"
                         << std::endl;

            short menu_option_num;
            std::cin >> menu_option_num;
            switch (menu_option_num)
            {
                case 1:
                {
                    // Для примера того, как может выглядить взаимодействие с сервером
                    // реализован один единственный метод - Hello.
                    // Этот метод получает от сервера приветствие с именем клиента,
                    // отправляя серверу id, полученный при регистрации.
                    SendMessage(s, my_id, Requests::Hello, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 2:
                {
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::string input;
                    std::cout << "Введите два числа, разделенных пробелом и тип сделки: \n"
                            << "Например: 10 50 Sell или 10 50 Buy\n";
                    std::getline(std::cin, input);

                    std::istringstream iss(input);
                    int volume, price;
                    std::string type_deel;
                    if (!(iss >> volume >> price >> type_deel)) {
                        std::cerr << "Ошибка! Неверный формат ввода." << "\n";
                        exit(0);
                    }
                    SendMessage(s, my_id, Requests::Trading, volume, price, type_deel);
                    std::cout << ReadMessage(s);
                    break;
                }
                case 3:
                {
                    SendMessage(s, my_id, Requests::Balance, "");
                    std::cout << ReadMessage(s);
                    break;
                }
                case 4:
                {
                    exit(0);
                    break;
                }
                default:
                {
                    std::cout << "Unknown menu option\n" << std::endl;
                }
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}