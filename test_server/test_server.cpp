#include <iostream> // Подключение библиотеки для работы с потоками ввода/вывода.
#include <Winsock2.h> // Подключение библиотеки для работы с сокетами Windows.
#include <Ws2tcpip.h> // Подключение библиотеки для работы с адресами IP.

#pragma comment(lib, "Ws2_32.lib") // Директива компилятору для связывания с библиотекой Ws2_32.lib.

// Объявление класса TCPServer для создания TCP сервера.
class TCPServer
{
private:
    SOCKET server_fd; // Сокет сервера.
    struct sockaddr_in address; // Структура адреса для сокета.

    int addrlen = sizeof(address); // Размер структуры адреса.
    const int port = 8080; // Номер порта, который будет слушать сервер.

public:
    // Конструктор класса TCPServer.
    TCPServer()
    {
        WSADATA wsaData; // Структура для хранения информации о реализации Windows Sockets.
        int opt = 1; // Переменная для установки параметров сокета.

        // Инициализация библиотеки Windows Sockets.
        // WSAStartup - функция для инициализации работы с сокетами в Windows.
        // MAKEWORD(2, 2) - макрос для создания номера версии Winsock, который мы хотим использовать (2.2).
        // &wsaData - указатель на структуру WSADATA, которая получит информацию о реализации сокетов Windows.
        // Функция возвращает 0 при успешной инициализации.
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        {
            std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl; // Вывод ошибки, если инициализация не удалась.
            exit(EXIT_FAILURE); // Завершение программы с кодом ошибки.
        }

        // Создание сокета.
        // socket - функция создания сокета.
        // AF_INET - семейство адресов IPv4.
        // SOCK_STREAM - тип сокета для обеспечения надежной, упорядоченной и безошибочной доставки данных (TCP).
        // 0 - протокол, выбирается автоматически.
        // INVALID_SOCKET - специальное возвращаемое значение, указывающее на ошибку.
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
        {
            std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
            WSACleanup(); // Очистка ресурсов Winsock.
            exit(EXIT_FAILURE);
        }

        // Настройка структуры адресов.
        address.sin_family = AF_INET; // Использование сети IPv4.
        address.sin_addr.s_addr = INADDR_ANY; // Автоматический выбор IP-адреса.
        address.sin_port = htons(port); // Установка порта для прослушивания, конвертация в сетевой порядок байт.

        // Установка опций сокета.
        // setsockopt - функция для установки опций сокета.
        // server_fd - дескриптор сокета.
        // SOL_SOCKET - уровень опций сокета.
        // SO_REUSEADDR - позволяет повторно использовать локальные адреса.
        // (char*)&opt - указатель на переменную с новым значением опции.
        // sizeof(opt) - размер переменной с опцией.
        // SOCKET_ERROR - возвращаемое значение при ошибке.
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR)
        {
            std::cerr << "setsockopt failed with error: " << WSAGetLastError() << std::endl;
            closesocket(server_fd); // Закрытие сокета.
            WSACleanup(); // Очистка ресурсов Winsock.
            exit(EXIT_FAILURE);
        }

        // Привязка сокета к IP-адресу и порту.
        // bind - функция привязывает сокет к конкретному IP-адресу и порту на локальном компьютере.
        // server_fd - дескриптор сокета, который был создан ранее.
        // (struct sockaddr*)&address - указатель на структуру sockaddr_in, которая содержит адрес, к которому будет привязан сокет.
        // sizeof(address) - размер структуры адреса. Необходим для корректной работы функции.
        // SOCKET_ERROR - константа, значение которой означает, что операция завершилась с ошибкой.
        if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR)
        {
            std::cerr << "Bind failed with error: " << WSAGetLastError() << std::endl; // Вывод сообщения об ошибке.
            closesocket(server_fd); // Закрытие сокета.
            WSACleanup(); // Освобождение ресурсов Winsock.
            exit(EXIT_FAILURE); // Завершение программы с кодом ошибки.
        }

        // Перевод сокета в режим прослушивания.
        // listen - функция переводит сокет в состояние прослушивания входящих соединений.
        // server_fd - дескриптор сокета, который будет прослушивать.
        // 3 - максимальное количество одновременных ожидающих соединений в очереди.
        if (listen(server_fd, 3) == SOCKET_ERROR)
        {
            std::cerr << "Listen failed with error: " << WSAGetLastError() << std::endl; // Сообщение об ошибке при неудаче.
            closesocket(server_fd); // Закрытие сокета в случае ошибки.
            WSACleanup(); // Освобождение ресурсов Winsock.
            exit(EXIT_FAILURE); // Завершение программы с кодом ошибки.
        }
    }

    // Метод для запуска сервера и обработки входящих подключений.
    void run() {
        std::cout << "Waiting for connections..." << std::endl; // Вывод сообщения о начале ожидания подключений.
        SOCKET client_socket; // Дескриптор сокета клиента.

        // accept - функция ожидает входящее подключение на сокете.
        // server_fd - дескриптор прослушивающего сокета.
        // (struct sockaddr*)&address - указатель на структуру, куда будет записан адрес подключившегося клиента.
        // (socklen_t*)&addrlen - указатель на переменную, в которую будет записан размер адресной структуры.
        if ((client_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) == INVALID_SOCKET)
        {
            std::cerr << "Accept failed with error: " << WSAGetLastError() << std::endl; // Сообщение об ошибке.
            closesocket(server_fd); // Закрытие сокета сервера.
            WSACleanup(); // Освобождение ресурсов Winsock.
            exit(EXIT_FAILURE); // Выход из программы с кодом ошибки.
        }

        char buffer[1024] = { 0 }; // Буфер для приема данных.

        // recv - функция получения данных из сокета.
        // client_socket - дескриптор сокета клиента, от которого принимаются данные.
        // buffer - указатель на буфер, куда будут записаны принятые данные.
        // sizeof(buffer) - размер буфера.
        // 0 - флаги, настройки функции recv (в данном случае флаги не используются).
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0); // Получение данных от клиента.

        if (bytes_received == SOCKET_ERROR)
        {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl; // Сообщение об ошибке при приеме данных.
            closesocket(client_socket); // Закрытие сокета клиента.
        }
        else
        {
            std::cout << "Message from client: " << buffer << std::endl; // Вывод сообщения от клиента.
        }

        closesocket(client_socket); // Закрытие сокета клиента после завершения обработки.
    }

    // Деструктор класса.
    ~TCPServer()
    {
        closesocket(server_fd); // Закрытие сокета сервера.
        WSACleanup(); // Очистка ресурсов Winsock.
    }
};

int main()
{
    TCPServer server; // Создание экземпляра сервера.
    server.run(); // Запуск сервера.
    return 0; // Возвращение кода успешного завершения.
}
