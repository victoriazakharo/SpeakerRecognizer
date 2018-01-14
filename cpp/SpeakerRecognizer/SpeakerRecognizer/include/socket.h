#pragma once

#include <stdlib.h>

#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501  // Windows XP. 
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment (lib, "Ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>  // Needed for getaddrinfo() and freeaddrinfo() 
#include <unistd.h> // Needed for close() 
typedef int SOCKET;
#endif

void startup_sockets() {
#ifdef _WIN32	
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {		
		exit(1);
	}
#endif
}

void cleanup_sockets() {
#ifdef _WIN32	
	WSACleanup();
#endif
}

void close_socket(SOCKET socket) {
#ifdef _WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

void handle_socket_error(SOCKET socket) {
	perror("socket error. ");
	close_socket(socket);
	cleanup_sockets();
	exit(1);
}

void handle_socket_error() {
	perror("socket error. ");
	cleanup_sockets();
	exit(1);
}

void shutdown_socket(SOCKET socket) {
#ifdef _WIN32
	int result = shutdown(socket, SD_BOTH);
	closesocket(socket);
	WSACleanup();
#else
	int result = shutdown(socket, SHUT_RDWR);
	close(socket);
#endif
	if (result != 0) {
		exit(1);
	}
}

int get_invalid_socket() {
#ifdef _WIN32
	return INVALID_SOCKET;
#else
	return -1;
#endif
}

void init_socket(SOCKET &server_socket) {
	const int DefaultPort = 1024;
	const int MaxConnections = 1;
	const int InvalidSocket = get_invalid_socket();

	int iResult;
	SOCKET listen_socket = InvalidSocket;
	server_socket = InvalidSocket;
	startup_sockets();
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == InvalidSocket) {
		handle_socket_error();
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(DefaultPort);

	iResult = bind(listen_socket, (struct sockaddr *)&server, sizeof(server));
	if (iResult != 0) {
		handle_socket_error(listen_socket);
	}
	iResult = listen(listen_socket, MaxConnections);
	if (iResult != 0) {
		handle_socket_error(listen_socket);
	}
	server_socket = accept(listen_socket, NULL, NULL);
	if (server_socket == InvalidSocket) {
		handle_socket_error(listen_socket);
	}
	close_socket(listen_socket);
}
