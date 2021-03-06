// MyClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define DEFALUT_BUFFLEN 1024
#define PORT 50278

unsigned _stdcall Server_Thread(void* lpParameter)
{
	SOCKET ConnectSocket = (SOCKET)lpParameter;

	// Send message to client
	int send_bytes;
	int send_total = 0;
	char sendBuf[DEFALUT_BUFFLEN] = "hello!";

	while (send_total < (int)strlen(sendBuf) + 1)
	{
		send_bytes = send(ConnectSocket, sendBuf + send_total, strlen(sendBuf) + 1 - send_total, 0);
		if (send_bytes == SOCKET_ERROR)
		{
			printf("Send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			return 1;
		}
		else
		{
			send_total += send_bytes;
		}
	}

	// Recive message from client
	int recv_bytes;
	char recvBuf[DEFALUT_BUFFLEN] = "";
	int recvBufLen = DEFALUT_BUFFLEN;
	recv_bytes = recv(ConnectSocket, recvBuf, recvBufLen, 0);
	if (recv_bytes > 0)
		printf("Recive %d bytes data from server: %s\n", recv_bytes, recvBuf);
	else if (recv_bytes == 0)
		printf("Connection closed.\n");
	else
	{
		wprintf(L"Recive failed with error: %ld\n", WSAGetLastError());
		closesocket(ConnectSocket);
		return 1;
	}

	// Close connection socket
	closesocket(ConnectSocket);

	return 0;
}

int main()
{
	printf("Client\n");

	int iResult;
	WSADATA wsaData;

	// Load socket dll
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}
	printf("Dll has benn initialized...\n");

	// Create listen socket
	SOCKET 	ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("Create listen socket failed with error: %d\n", WSAGetLastError());
		WSACleanup();	// Unload
		return 1;
	}
	printf("Create listen socket...\n");

	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_addr.S_un.S_addr = inet_addr("10.122.227.20");
	addrServer.sin_port = htons(PORT);

	printf("Try to connect to server...\n");
	int ret = connect(ListenSocket, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));
	if (ret == SOCKET_ERROR)
	{
		printf("Connect faild with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();	// Unload
		return 1;
	}
	printf("Connect to server successed...\n");

	Server_Thread((void*)ListenSocket);

	WSACleanup();

    return 0;
}

