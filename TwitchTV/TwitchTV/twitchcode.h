#pragma once

#pragma once

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <time.h>
#include <iostream>
#include <fstream>
#include <assert.h>
using namespace std;


//TCP/IP
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")


namespace Simple_TCP
{
	void terminate(SOCKET _socket);

	void init_Network()
	{
		WSADATA wsadata;
		int r = WSAStartup(MAKEWORD(2, 2), &wsadata);
		assert(r == 0);
		Sleep(200);
	}

	hostent *get_Host(const char *hostname)
	{
		int n_tries = 0;
		hostent *host = NULL;
		while (host == NULL)
		{
			host = gethostbyname(hostname);
			if (++n_tries >= 10) assert(0);
			Sleep(100);
		}
		return host;
	}

	SOCKET make_Nonblocking_TCP_Connection(hostent *host, int port)
	{
		SOCKET _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_socket == INVALID_SOCKET)
		{
			printf("socket failure\n");
			assert(0);
		}
		unsigned long mode = 1;
		int r = ioctlsocket(_socket, FIONBIO, &mode);
		if (r != 0)
		{
			printf("ioctlsocket failed: %d\n", r);
			assert(0);
		}

		SOCKADDR_IN sockaddr;
		sockaddr.sin_port = htons(port);
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

		for (;;)
		{
			int r = connect(_socket, (SOCKADDR*)(&sockaddr), sizeof(SOCKADDR_IN));
			if (r == 0)
			{
				printf("connected to: %s\n", host->h_name);
				break;
			}
			else if (r < 0)
			{
				int error_code = WSAGetLastError();
				if (error_code == WSAEISCONN)
				{
					printf("connected to: %s\n", host->h_name);
					break;
				}
				else if (error_code == WSAEWOULDBLOCK)
				{
					printf("waiting for network interface\n");
					Sleep(200);
				}
				else if (error_code == WSAEALREADY)
				{
					printf("initiating connection\n");
					Sleep(200);
				}
				else
				{
					printf("error_code: %d\n", error_code);
					terminate(_socket);
					assert(0);
				}
			}
			else
			{
				printf("could not connect to: %s\n", host->h_name);
				terminate(_socket);
				assert(0);
			}
		}

		return _socket;
	}

	int read(char *buffer, int buffer_size, SOCKET _socket)
	{
		memset(buffer, 0, buffer_size);
		//read into buffer
		int numbytes = recv(_socket, buffer, buffer_size, 0);
		if (numbytes < 0)
		{
			//nothing received
			int error_code = WSAGetLastError();
			if (error_code != WSAEWOULDBLOCK)
			{
				printf("WSAGetLastError: %d\n", error_code);
			}
			return -1;
		}

		if (numbytes == 0)
		{
			printf("connection terminated\n");
			terminate(_socket);
			return 0;
		}
		buffer[numbytes] = '\0';

		return numbytes;
	}

	void close(SOCKET _socket)
	{
		shutdown(_socket, SD_SEND);
		closesocket(_socket);
	}

	void terminate(SOCKET _socket)
	{
		WSACleanup();
	}
}
namespace Twitch
{
	const int buffer_size = 8192;
	struct Connection
	{
		char *username;
		char *oauth;
		char buffer[buffer_size];
		SOCKET socket;
		bool active;
	};

	void init(Connection *c, char *username, char *oauth)
	{
		c->username = username;
		c->oauth = oauth;
		c->socket = INVALID_SOCKET;
		c->active = false;
	}

	namespace Message
	{
		struct Table
		{
			char **channel;
			char **username;
			char **message;
			unsigned int *timestamp;
			int n_count;
			int array_size;
		};

		void init(Table *t)
		{
			t->array_size = 100;
			t->n_count = 0;
			t->channel = (char**)malloc(sizeof(char*)*t->array_size); assert(t->channel);
			t->username = (char**)malloc(sizeof(char*)*t->array_size); assert(t->username);
			t->message = (char**)malloc(sizeof(char*)*t->array_size); assert(t->message);
			t->timestamp = (unsigned int*)malloc(sizeof(unsigned int)*t->array_size); assert(t->timestamp);
			for (int i = 0; i < t->array_size; i++)
			{
				t->channel[i] = (char*)malloc(64); assert(t->channel[i]);
				t->username[i] = (char*)malloc(64); assert(t->username[i]);
				t->message[i] = (char*)malloc(1024); assert(t->message[i]);//IRC protocol limit is 512
				t->timestamp[i] = 0;
			}
		}

		void resize(Table *t)
		{
			int prev_array_size = t->array_size;
			t->array_size *= 1.2;
			t->array_size++;

			t->channel = (char**)realloc(t->channel, sizeof(char*)*t->array_size); assert(t->channel);
			t->username = (char**)realloc(t->username, sizeof(char*)*t->array_size); assert(t->username);
			t->message = (char**)realloc(t->message, sizeof(char*)*t->array_size); assert(t->message);
			t->timestamp = (unsigned int*)realloc(t->timestamp, sizeof(unsigned int)*t->array_size); assert(t->timestamp);
			for (int i = prev_array_size; i < t->array_size; i++)
			{
				t->channel[i] = (char*)malloc(64); assert(t->channel[i]);
				t->username[i] = (char*)malloc(64); assert(t->username[i]);
				t->message[i] = (char*)malloc(1024); assert(t->message[i]);//IRC protocol limit is 512
				t->timestamp[i] = 0;
			}
		}

		void clear(Table *t)
		{
			t->n_count = 0;
		}

		void add(Table *t, char *channel, char *username, char *msg, unsigned int timestamp)
		{
			if (t->n_count >= t->array_size) resize(t);

			strcpy(t->channel[t->n_count], channel);
			strcpy(t->username[t->n_count], username);
			strcpy(t->message[t->n_count], msg);
			t->timestamp[t->n_count] = timestamp;
			++t->n_count;
		}

		void append(Table *dest, Table *src)
		{
			for (int i = 0; i < src->n_count; i++)
			{
				add(dest, src->channel[i], src->username[i], src->message[i], src->timestamp[i]);
			}
		}

		void copy(Table *dest, Table *src)
		{
			clear(dest);
			append(dest, src);
		}
	}

	//call once to initialize network
	void startup()
	{
		Simple_TCP::init_Network();
	}

	//close active socket
	void close(Connection *c)
	{
		Simple_TCP::close(c->socket);
	}

	//creates a socket and makes a TCP connection
	//shutsdown connection if it already exists and reconnects
	void connect(Connection *c)
	{
		if (c->socket != INVALID_SOCKET) close(c);

		const char *hostname = "irc.chat.twitch.tv";
		int port = 6667;

		hostent *host = Simple_TCP::get_Host(hostname);
		Sleep(200);

		c->socket = Simple_TCP::make_Nonblocking_TCP_Connection(host, port);
		Sleep(200);

		int r;
		char tmp[1024];
		sprintf(tmp, "PASS oauth:%s\r\n", c->oauth);
		r = send(c->socket, tmp, strlen(tmp), 0);
		sprintf(tmp, "NICK %s\r\n", c->username);
		r = send(c->socket, tmp, strlen(tmp), 0);
	}

	//update tcp buffer, responds to pings immediately
	//returns number of bytes read from socket
	int update(Connection *c)
	{
		c->active = true;
		int numbytes = Simple_TCP::read(c->buffer, buffer_size, c->socket);

		if (numbytes == 0)
		{
			c->active = false;
			return 0;
		}
		else if (numbytes < 0) return 0;

		return numbytes;
	}

	//joins a channel
	//static string read/write inside
	void join_Channel(Connection *c, const char *channel)
	{
		static char join_command[4096];
		sprintf(join_command, "JOIN #%s\r\n", channel);
		int r = send(c->socket, join_command, strlen(join_command), 0);
	}

	//send message to channel
	//static string read/write inside
	void send_Message(Connection *c, const char *channel, const char *msg)
	{
		static char command[4096];
		sprintf(command, "PRIVMSG #%s :%s\r\n", channel, msg);
		int r = send(c->socket, command, strlen(command), 0);
	}

	//populates message table with unread messages from the chat
	//returns false if connection is closed
	void communicate(Message::Table *t, Connection *c, unsigned int timestamp)
	{
		Message::clear(t);

		int r = update(c);
		if (c->active == false) return;
		if (r == 0) return;

		//check each line for ping, system or privmsg
		char *line = strtok(c->buffer, "\n");
		for (; line != NULL;)
		{
			const char *ping_msg = "PING :tmi.twitch.tv";
			if (strstr(line, ping_msg) != NULL)
			{
				const char *pong_msg = "PONG :tmi.twitch.tv\r\n";
				int r = send(c->socket, pong_msg, strlen(pong_msg), 0);
			}
			else if (strstr(line, "PRIVMSG") != NULL)
			{
				static char channel[64];
				static char username[64];
				static char message[1024];
				sscanf(line, ":%[^!]%*[^#]#%[^ ]%*[^:]:%[^\n]", username, channel, message);
				Message::add(t, channel, username, message, timestamp);
			}
			else
			{
				//do nothing
			}

			line = strtok(NULL, "\n");
		}

		return;
	}
}