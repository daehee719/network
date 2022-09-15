#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#include <thread>
#include <vector>
#include <iostream>
#include <string>

using namespace std;
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

struct client_type
{
    int id;
    SOCKET socket;
};

const char OPTION_VALUE = 1;
const int MAX_CLIENTS = 5;

int process_client(client_type& new_client, vector<client_type>& client_array, thread& thread);

int process_client(client_type& new_client, vector<client_type>& client_array, thread& thread)
{

    string msg = "";
    char tempmsg[DEFAULT_BUFLEN] = "";
    while (1)
    {
        memset(tempmsg, 0, DEFAULT_BUFLEN);
        if (new_client.socket != 0)
        {
            int iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);

            if (iResult != SOCKET_ERROR)
            {
                if (strcmp("", tempmsg))
                {
                    msg = "Client #" + to_string(new_client.id)+": "+tempmsg;
                }
                cout << msg.c_str() << endl;

                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (new_client.id != i)
                    {
                        iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
                    }
                }
            }
            else
            {
                msg = "Client #" + to_string(new_client.id) + " Disconnected";
                cout << msg << endl;
                closesocket(new_client.socket);
                closesocket(client_array[new_client.id].socket);
                client_array[new_client.id].socket = INVALID_SOCKET;

                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (client_array[i].socket != INVALID_SOCKET)
                    {
                        iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
                    }
                }
                break;
            }
        }
    }
    thread.detach();
    return 0;
}


int main()
{
    WSADATA wsaData;
    int iResult;

    SOCKET server_socket = INVALID_SOCKET;


    struct addrinfo* server = NULL;
    struct addrinfo hints;

    std::string msg = "";
    vector<client_type> client(MAX_CLIENTS);
    int num_clients = 0;
    int temp_id = -1;
    thread my_thread[MAX_CLIENTS];


    // Initialize Winsock
    cout << "Initializing winsock...." << endl;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;


    cout << "Setting up server...." << endl;
    getaddrinfo(NULL, DEFAULT_PORT, &hints, &server);

    cout << "Creating server socket...." << endl;
    server_socket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &OPTION_VALUE, sizeof(int));
    setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &OPTION_VALUE, sizeof(int));

    cout << "Binding socket...." << endl;
    bind(server_socket, server->ai_addr, (int)server->ai_addrlen);

    cout << "Listening...." << endl;
    listen(server_socket, SOMAXCONN);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client[i] = { -1, INVALID_SOCKET };
    }

    cout << "Accepting...." << endl;
    while (1)
    {
        SOCKET incoming = INVALID_SOCKET;
        incoming = accept(server_socket, NULL, NULL);

        if (incoming == INVALID_SOCKET)
        {
            continue;
        }
        num_clients = -1;
        temp_id = -1;

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client[i].socket == INVALID_SOCKET && temp_id == -1)
            {
                client[i].socket = incoming;
                client[i].id = i;
                temp_id = i;
            }
            if (client[i].socket != INVALID_SOCKET)
            {
                num_clients++;
            }
        }
        if (temp_id != -1)
        {
            cout << "Client #" << client[temp_id].id << " is Connecting";
            msg = to_string(client[temp_id].id);
            send(client[temp_id].socket,msg.c_str(),strlen(msg.c_str()),0);

            my_thread[temp_id] = thread(process_client, ref(client[temp_id]), ref(client), ref(my_thread[temp_id]));
        }
        else
        {
            msg = "Server is full";
            send(incoming, msg.c_str(), strlen(msg.c_str()), 0);
            cout << msg << endl;
        }
    }

    // No longer need server socket
    closesocket(server_socket);

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        my_thread[i].detach();
        closesocket(client[i].socket);
    }


    

    // cleanup
    WSACleanup();
    cout << "Program has ended successfully" << endl;

    return 0;
}