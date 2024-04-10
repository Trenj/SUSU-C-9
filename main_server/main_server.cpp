// Сервер
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <Winsock2.h>

#pragma comment(lib, "Ws2_32.lib")

// Обработчик запроса клиента
void client_handler(SOCKET client_socket) {
    while (true) {
        std::string filename;
        char buffer[1024] = { 0 };

        // Получаем имя файла от клиента
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Добавляем нулевой символ в конец строки
            filename = std::string(buffer);

            // Добавляем путь к папке с файлами к имени файла
            filename = "C:\\Users\\Trenj\\source\\repos\\SUSU-C-9\\main_server\\files\\" + filename;

                // Открываем файл
                std::ifstream file(filename);
            if (file.is_open()) {
                std::stringstream file_content;
                file_content << file.rdbuf();
                file.close();

                // Отправляем содержимое файла клиенту
                std::string file_data = file_content.str();
                send(client_socket, file_data.c_str(), static_cast<int>(file_data.size()), 0);
            }
            else {
                // Если файл не может быть открыт, отправляем ошибку
                std::string error_msg = "Error: file not found";
                send(client_socket, error_msg.c_str(), static_cast<int>(error_msg.size()), 0);
            }
        }
        else if (bytesReceived == 0) {
            std::cout << "Connection closing..." << std::endl;
            break;
        }
        else {
            std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    closesocket(client_socket);
}



int main() {
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return 1;
    }

    SOCKET server_fd, new_socket;
    struct sockaddr_in address;
    int addr_len = sizeof(address);

    // Создаем файловый дескриптор серверного сокета
    server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "socket failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Настраиваем адрес сервера
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(8080);

    // Привязываем сокет к адресу
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Прослушиваем входящие соединения
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // Запуск основного цикла принятия клиентов
    while (true) {
        new_socket = accept(server_fd, (struct sockaddr*)&address, &addr_len);
        if (new_socket == INVALID_SOCKET) {
            std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
            continue;
        }

        std::thread(client_handler, new_socket).detach();
    }

    // Закрытие серверного сокета и очистка Winsock
    closesocket(server_fd);
    WSACleanup();

    return 0;
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