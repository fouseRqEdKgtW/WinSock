// MyServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#define DEFALUT_BUFFLEN 1024
#define PORT 50278

unsigned _stdcall Client_Thread(void* lpParameter)
{
	SOCKET ConnectSocket = (SOCKET)lpParameter;

	// Send message to client
	int send_bytes;
	int send_total = 0;
	char sendBuf[DEFALUT_BUFFLEN] = "welcome!";

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
		printf("Recive %d bytes data from client: %s\n", recv_bytes, recvBuf);
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
	printf("Sever\n");

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
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
	addrServer.sin_port = htons(PORT);

	// Bind local ip and port
	iResult = bind(ListenSocket, (sockaddr*)&addrServer,
		sizeof(sockaddr));
	if (iResult == SOCKET_ERROR)
	{
		printf("Bind failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	printf("Bind has benn created...\n");

	// Start listen
	iResult = listen(ListenSocket, 0);
	if (iResult == SOCKET_ERROR)
	{
		printf("Start listening failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	sockaddr_in addrClient;
	int len = sizeof(sockaddr_in);

	printf("Waiting for client...\n");
	while (true)
	{
		// Accept the clients' connection requests
		SOCKET ConnectSocket = accept(ListenSocket, (sockaddr*)&addrClient, &len);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}
		char buf[DEFALUT_BUFFLEN] = { 0 };
		fprintf(stdout, "Accept a connection, from %s:%d\n", inet_ntop(AF_INET, (VOID*)&addrClient.sin_addr,
			buf, DEFALUT_BUFFLEN), ntohs(addrClient.sin_port));

		// Create thread to handle communications
		HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, Client_Thread, (void*)ConnectSocket, 0, NULL);
		CloseHandle(hThread);
	}

	// Close listen socket
	closesocket(ListenSocket);

	// Release socket dll
	WSACleanup();

	return 0;
}