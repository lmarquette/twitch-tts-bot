﻿#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <chrono>
#include <time.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <algorithm>
using namespace std;

#include "twitchcode.h"
#include "stringvector.h"

#include <sapi.h>
#include <sphelper.h>
#include <atlbase.h>

#pragma warning(disable : 4996)


int main(int argc, char **argv)
{
	//initialize network
	Twitch::startup();

	//init user name, oauth
	Twitch::Connection connection;
	Twitch::init(&connection, "guildude", "xeaytdk3txo699ys245dexpru1ijd3");

	//make TCP connection to twitch and login
	Twitch::connect(&connection);

	//join a channel
	Twitch::join_Channel(&connection, "guildude");

	//incoming message list from all connected channels
	Twitch::Message::Table incoming;
	Twitch::Message::init(&incoming);

	twitch_messages::data twitch_msg;
	twitch_messages::init(&twitch_msg);

	StringVector::data actions;
	StringVector::init(&actions);

	StringVector::data recipe;
	StringVector::init(&recipe);

	TrackNames::data track_names;
	TrackNames::init(&track_names);

	StringVector::add(&actions, "AAAAAAACHOP");
	StringVector::add(&actions, "Gently stroke");
	StringVector::add(&actions, "Rub tenderly");
	StringVector::add(&actions, "RKO");
	StringVector::add(&actions, "Boil");
	StringVector::add(&actions, "Slowly insert");

	StringVector::add(&recipe, "Chicken");
	StringVector::add(&recipe, "Human toe");
	StringVector::add(&recipe, "Broccoli");
	StringVector::add(&recipe, "Hot Dog");
	StringVector::add(&recipe, "Steak");
	StringVector::add(&recipe, "Vegetables");
	StringVector::add(&recipe, "General Tso");

	
	//https://stackoverflow.com/questions/3220477/how-to-use-clock-in-c examples

	int t1 = clock();
	int t2 = clock();
	double time_passed = (t2 - t1) / (double)CLOCKS_PER_SEC;

	
	bool runTime = true;

	
	

	printf("chat log\n");	
	for (;;)
	{
		

		srand(time(0));
		unsigned int timestamp = clock();

		//collect all messages from all channels
		Twitch::communicate(&incoming, &connection, timestamp);
		if (connection.active == false)
		{
			printf("connection was closed!\n");
			break;
		}

		//print received messages from all channels
		for (int i = 0; i < incoming.n_count; i++)
		{


			//printf("%s@%s|(%.2f)->%s\n", incoming.username[i], incoming.channel[i], (double)timestamp / CLOCKS_PER_SEC, incoming.message[i]);

			//printf("message length: %d\n", strlen(incoming.message[i]));

			/*if (strcmp("!test\r", incoming.message[i]) == 0)
			{
				Twitch::send_Message(&connection, "guildude", "test received");
				Sleep(1000);
			}*/

			/*
			char *ret = strstr(incoming.message[i], "recipe"); //strstr checks inside a string for the specific word
			if (ret != NULL)
			{
				Sleep(2000);
				char tmp[512];
				sprintf(tmp, "Chef Ramsey says you should %s the %s", actions.str[rand() % actions.n_count], recipe.str[rand() % actions.n_count]);

				Twitch::send_Message(&connection, "guildude", tmp);

			}

			char *name = strstr(incoming.message[i], "name");
			if (name != NULL)
			{
				TrackNames::add(&track_names, incoming.username[i]);
				if (incoming.username[i] != track_names.name[i])
				{
					Sleep(2000);
					char name_tmp[512];
					sprintf(name_tmp, "@%s our twitch overlords demand you sacrifice a meme to the meme gods.", incoming.username[i]);

					Twitch::send_Message(&connection, "guildude", name_tmp);
				}
			}*/

			if (strcmp("!who\r", incoming.message[i]) == 0)
			{
				Sleep(1000);
				char tmp[512];
				sprintf(tmp, "Hello @%s this is leo%d", incoming.username[i], rand() % 256); //%s points towards a string, %d points towards random numbers

				Twitch::send_Message(&connection, "guildude", tmp);



				cout << "Time: " << time << endl;

			}

			cout << "Time: " << time_passed << endl;

			//retrieve messages from twitch
			printf("%s@%s|(%.2f)->%s\n", incoming.username[i], incoming.channel[i], (double)timestamp / CLOCKS_PER_SEC, incoming.message[i]);

			//tokenize messages
			char *delimiters = " .,!\n\r?";
			char tmp_msg[512];

			strcpy(tmp_msg, incoming.message[i]);
			char *token = strtok(tmp_msg, delimiters);

			while (token != NULL)
			{
				printf("%s\n", token);

				//append words to a master list
				twitch_messages::add(&twitch_msg, token);
				token = strtok(NULL, delimiters);
			}

			sort(&twitch_msg);

			//update
			if (time_passed == 5)
			{
				sort(&twitch_msg);
			}
		}

	
	}

	getchar();
}
