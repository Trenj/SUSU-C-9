// Клиент
#include <boost/asio.hpp>
#include <iostream>

int main() 
{
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 12345));

    std::string file_name;
    std::cout << "Enter file name: ";
    std::getline(std::cin, file_name);
    boost::asio::write(socket, boost::asio::buffer(file_name + "\n", file_name.size() + 1));

    char buffer[1024];
    size_t bytes_transferred = boost::asio::read(socket, boost::asio::buffer(buffer), boost::asio::transfer_all());
    std::cout << buffer << std::endl;
}