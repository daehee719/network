#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <thread>
#include <string>
#include <iostream>

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN  512
#define DEFAULT_PORT    "27015"
#define IP_ADDRESS		"localhost"


struct client_type
{
	int id;
	SOCKET socket;
	char received_msg[DEFAULT_BUFLEN];
};

int process_client(client_type& new_client)
{
	// Receive until the peer closes the connection
	while (1)
	{
		memset(new_client.received_msg, 0, DEFAULT_BUFLEN);

		if (new_client.socket != 0)
		{
			int iResult = recv(new_client.socket, new_client.received_msg, DEFAULT_BUFLEN, 0);

			if (iResult != SOCKET_ERROR)
			{
				cout << new_client.received_msg << endl;
			}
			else
			{
				cout << "recv() failed: " << WSAGetLastError() << endl;
				break;
			}
		}
	}

	if (WSAGetLastError() == WSAECONNRESET)
	{
		cout << "The server has disconnected." << endl;
	}

	return 0;
}

int main()
{
	WSADATA wsaData;
	struct addrinfo* result = NULL, * ptr = NULL, hints;
	string sent_msg = "";
	client_type client = { -1, INVALID_SOCKET, "" };
	int iResult = 0;
	string message;


	cout << "Starting Client..." << endl;


	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0)
	{
		printf("WSAStartup failed with error: %d\n", iResult);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;


	cout << "Connecting..." << endl;


	// Resolve the server address and port
	iResult = getaddrinfo(static_cast<PCSTR>(IP_ADDRESS), DEFAULT_PORT, &hints, &result);
	if (iResult != 0)
	{
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		client.socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (client.socket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server.
		iResult = connect(client.socket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR)
		{
			closesocket(client.socket);
			client.socket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (client.socket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}

	cout << "Successfully Connected!" << endl;

	recv(client.socket, client.received_msg, DEFAULT_BUFLEN, 0);
	message = client.received_msg;

	if (message != "Server is full")
	{
		client.id = atoi(client.received_msg);
		thread my_thread = thread(process_client, ref(client));

		while (1)
		{
			getline(cin, sent_msg);
			iResult = send(client.socket, sent_msg.c_str(), strlen(sent_msg.c_str()), 0);

			if (iResult <= 0)
			{
				printf("shutdown failed with error: %d\n", WSAGetLastError());
				break;
			}
		}

		my_thread.detach();
	}
	else
	{
		cout << client.received_msg << endl;
	}

	// shutdown the connection since no more data will be sent

	cout << "Shutting down socket..." << endl;

	iResult = shutdown(client.socket, SD_SEND);

	if (iResult == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(client.socket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(client.socket);
	WSACleanup();

	return 0;
}