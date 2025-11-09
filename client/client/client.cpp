#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // 本地测试，跨设备改服务器局域网 IP（如 192.168.1.100）
#define SERVER_PORT 8888
#define BUF_SIZE 1024

int main() {
    // 1. 初始化 Windows Socket
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Socket 初始化失败：" << WSAGetLastError() << std::endl;
        return -1;
    }

    // 2. 创建客户端 Socket
    SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock == INVALID_SOCKET) {
        std::cerr << "创建客户端 Socket 失败：" << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    // 3. 连接服务器
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); // 解析 IP
    if (connect(client_sock, (sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "连接服务器失败：" << WSAGetLastError() << std::endl;
        closesocket(client_sock);
        WSACleanup();
        return -1;
    }
    std::cout << "连接服务器成功！（输入 exit 退出）" << std::endl;

    // 4. 收发数据
    char buf[BUF_SIZE];
    while (true) {
        // 客户端发送消息
        std::cout << "客户端发送：";
        std::string msg;
        std::getline(std::cin, msg);
        if (msg == "exit") break;
        send(client_sock, msg.c_str(), msg.size(), 0);

        // 接收服务器回复
        memset(buf, 0, BUF_SIZE);
        int recv_len = recv(client_sock, buf, BUF_SIZE, 0);
        if (recv_len <= 0) {
            std::cout << "服务器断开连接" << std::endl;
            break;
        }
        std::cout << "服务器说：" << buf << std::endl;
    }

    // 5. 关闭资源
    closesocket(client_sock);
    WSACleanup();
    std::cout << "客户端关闭" << std::endl;
    return 0;
}