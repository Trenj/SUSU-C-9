// Клиент
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
    std::string filename;
    char buffer[1024];

    while (true) {
        // Получаем имя файла от клиента
        int bytesReceived = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '0'; // Добавляем нулевой символ в конец строки
            filename = std::string(buffer);

            // Открываем файл
            std::ifstream file(filename);
            if (file.is_open()) {
                std::stringstream file_content;
                file_content << file.rdbuf();
                file.close();

                // Отправляем содержимое файла клиенту
                std::string file_data = file_content.str();
                int bytesSent = send(client_socket, file_data.c_str(), static_cast<int>(file_data.size()), 0);

                if (bytesSent == SOCKET_ERROR) {
                    std::cerr << "send failed: " << WSAGetLastError() << std::endl;
                    break;
                }
            }
            else {
                // Если файл не может быть открыт, отправляем ошибку
                std::string error_msg = "Error: file not foundn";
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

// остальная часть вашего кода без изменений...



