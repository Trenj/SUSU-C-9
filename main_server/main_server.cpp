// Сервер
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <fstream>
#include <string>

void handle_client(boost::asio::ip::tcp::socket socket) 
{
    std::string file_name;
    boost::asio::read_until(socket, boost::asio::dynamic_buffer(file_name), "\n");

    std::ifstream file(file_name);
    if (!file.is_open()) {
        boost::asio::write(socket, boost::asio::buffer("File not found\n", 17));
    }
    else {
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        boost::asio::write(socket, boost::asio::buffer(content + "\n", content.size() + 1));
    }
}

int main() {
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::acceptor acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 12345));

    while (true) {
        boost::asio::ip::tcp::socket socket(io_service);
        acceptor.accept(socket);
        boost::thread(handle_client, std::move(socket)).detach();
    }
}

/*
Самостоятельная задача: Многопоточный сетевой файловый сервер и постоянно работающий клиент на C++

Разработайте усовершенствованное клиент-серверное приложение на C++, 
где сервер будет обрабатывать запросы от нескольких клиентов одновременно, 
используя многопоточность. Каждый клиент должен иметь возможность отправлять серверу название файла, 
который он хочет прочитать. Сервер, получив название файла от клиента, 
должен искать этот файл в своей файловой системе, читать его содержимое и отправлять обратно клиенту. 
Клиент, в свою очередь, должен отображать полученное содержимое файла.
*/