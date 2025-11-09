#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT 8888
#define BUF_SIZE 1024

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Socket 初始化失败：" << WSAGetLastError() << std::endl;
        return -1;
    }

    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        std::cerr << "创建监听 Socket 失败：" << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "绑定地址失败：" << WSAGetLastError() << std::endl;
        closesocket(listen_sock);
        WSACleanup();
        return -1;
    }

    if (listen(listen_sock, 5) == SOCKET_ERROR) {
        std::cerr << "开启监听失败：" << WSAGetLastError() << std::endl;
        closesocket(listen_sock);
        WSACleanup();
        return -1;
    }

    std::cout << "服务器开始监听... 等待客户端连接（按 Ctrl+C 退出）" << std::endl;

    sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    SOCKET client_sock = accept(listen_sock, (sockaddr*)&client_addr, &client_addr_len);
    if (client_sock == INVALID_SOCKET) {
        std::cerr << "接收客户端连接失败：" << WSAGetLastError() << std::endl;
        closesocket(listen_sock);
        WSACleanup();
        return -1;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "客户端 " << client_ip << " 连接成功！" << std::endl;

    char buf[BUF_SIZE];
    while (true) {
        memset(buf, 0, BUF_SIZE);

        int recv_len = recv(client_sock, buf, BUF_SIZE, 0);
        if (recv_len <= 0) {
            std::cout << "客户端 " << client_ip << " 断开连接（或接收失败）" << std::endl;
            break;
        }
        std::cout << "客户端 " << client_ip << " 说：" << buf << std::endl;

        std::string reply = "服务器已收到：" + std::string(buf);
        send(client_sock, reply.c_str(), reply.size(), 0);
    }

    closesocket(client_sock);
    closesocket(listen_sock);
    WSACleanup();
    std::cout << "服务器关闭成功！" << std::endl;

    return 0;
}