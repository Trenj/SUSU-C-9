// Клиент 2
#include <iostream>
#include <Winsock2.h>
#include <Ws2tcpip.h>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;

    // Инициализация Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // Создание сокета
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);
    // используем localhost
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Подключение к серверу
    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed with error: " << WSAGetLastError() << "\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::string filename;
    while (true) {
        std::cout << "Enter the filename you want to retrieve or type 'exit' to quit: ";
        std::getline(std::cin, filename);

        if (filename == "exit") {
            break;
        }

        // Отправка имени файла серверу
        if (send(sock, filename.c_str(), static_cast<int>(filename.length()), 0) < 0) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << "\n";
            break;
        }

        std::cout << "Filename sent\n";

        // Получение содержимого файла от сервера
        char buffer[1024] = { 0 };
        int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            std::cout << "Received file content:\n";
            std::cout.write(buffer, bytesReceived);
            std::cout << "\n\n";
        }
        else if (bytesReceived == 0) {
            std::cout << "Connection closed by server.\n";
            break;
        }
        else {
            std::cerr << "Recv failed with error: " << WSAGetLastError() << "\n";
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
