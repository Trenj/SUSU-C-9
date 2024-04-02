﻿#include <iostream> // Включение стандартной библиотеки для ввода и вывода в C++.
#include <Winsock2.h> // Включение библиотеки Winsock2 для работы с сетевыми функциями на Windows.
#include <Ws2tcpip.h> // Включение дополнительной библиотеки для работы с IP адресами.

#pragma comment(lib, "Ws2_32.lib") // Инструкция для компоновщика о необходимости подключения библиотеки Ws2_32.lib.

// Объявление класса TCPClient, предназначенного для создания TCP клиента.
class TCPClient
{
private:
    SOCKET sock; // Объявление сокета для клиента.
    struct sockaddr_in serv_addr; // Структура, содержащая информацию об адресе сервера.
    const char* message = "Hello from client"; // Сообщение, которое будет отправлено серверу.

public:
    // Конструктор класса, принимающий IP адрес и порт сервера.
    TCPClient(const char* address, int port)
    {
        WSADATA wsaData; // Структура для хранения данных о Windows Sockets.
        // Инициализация библиотеки Winsock.
        // WSAStartup инициализирует использование Winsock DLL со спецификацией версии 2.2.
        // MAKEWORD(2, 2) создает номер версии Winsock, 2.2 в данном случае.
        // &wsaData - указатель на структуру WSADATA, которая будет заполнена информацией о Winsock.
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl; // Вывод сообщения об ошибке при неудачной инициализации.
            exit(EXIT_FAILURE); // Выход из программы с ошибкой.
        }

        // Создание сокета для клиента.
        // Функция socket создает сокет, который является конечной точкой для общения в сети.
        // AF_INET означает, что используется сеть для коммуникации с IPv4 адресами.
        // SOCK_STREAM указывает на использование протокола TCP, обеспечивающего надежную потоковую передачу данных.
        // Последний аргумент 0 указывает, что используется протокол по умолчанию для AF_INET, SOCK_STREAM, то есть TCP.
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl; // Сообщение об ошибке в случае неудачи создания сокета.
            WSACleanup(); // Вызов WSACleanup для освобождения ресурсов Winsock, занятых приложением.
            exit(EXIT_FAILURE); // Завершение программы с кодом ошибки, указывающим на неудачное выполнение.
        }

        // Задание параметров адреса сервера.
        serv_addr.sin_family = AF_INET; // Указание семейства адресов IPv4.
        // htons конвертирует номер порта из хостового порядка байт в сетевой порядок байт, что необходимо для корректной работы сетевых функций.
        serv_addr.sin_port = htons(port);

        // Преобразование текстового IP-адреса в двоичный формат.
        // Функция inet_pton преобразует IP-адреса из текстового представления (например, "127.0.0.1") в числовое (структура in_addr).
        // AF_INET указывает на то, что преобразование производится для IPv4 адреса.
        // В случае успеха inet_pton возвращает 1, в случае ошибки - 0 или -1.
        if (inet_pton(AF_INET, address, &serv_addr.sin_addr) <= 0) {
            std::cerr << "Invalid address: Address not supported." << std::endl; // Вывод сообщения об ошибке при неверном адресе.
            closesocket(sock); // Закрытие сокета в случае ошибки.
            WSACleanup(); // Освобождение ресурсов Winsock.
            exit(EXIT_FAILURE); // Выход из программы с ошибкой.
        }

        // Установление соединения с сервером.
        // Функция connect инициирует установление соединения на сокете, указывая на сокет клиента, адрес сервера и размер структуры адреса.
        // Соединение является двусторонним, и после успешного выполнения, клиент может отправлять данные и получать ответы.
        // Если connect возвращает значение меньше 0, это означает ошибку при попытке соединения.
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
            std::cerr << "Connection Failed." << std::endl; // Вывод сообщения о неудаче при попытке соединения.
            closesocket(sock); // Закрытие сокета в случае неудачи.
            WSACleanup(); // Очистка ресурсов Winsock.
            exit(EXIT_FAILURE); // Завершение работы программы из-за неудачи соединения.
        }

        // Отправка сообщения серверу.
        // send отправляет данные (message) через сокет.
        // strlen(message) - длина сообщения для отправки.
        // 0 - флаги; здесь не используются.
        send(sock, message, strlen(message), 0);
        std::cout << "Message sent" << std::endl; // Подтверждение отправки сообщения.
        closesocket(sock); // Закрытие сокета после отправки сообщения.
        WSACleanup(); // Очистка ресурсов Winsock.
    }
};

int main()
{
    TCPClient client("127.0.0.1", 8080); // Создание экземпляра клиента и подключение к серверу на локальном хосте с портом 8080.
    return 0; // Возвращение 0 указывает на успешное завершение программы.
}
