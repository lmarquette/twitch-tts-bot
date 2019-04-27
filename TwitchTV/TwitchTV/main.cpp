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

#include "SDL2-2.0.9/include/SDL.h"
#include "SDL2-2.0.9/include/SDL_image.h"

#include "SDL2-2.0.9\include\SDL.h"
#include "SDL2-2.0.9\include\SDL_image.h"

#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2main.lib")
//copy the SDL2_image.lib from the SDL imaege folder to your SDL lib folder
#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2_image.lib")

#pragma comment(linker,"/subsystem:console")

#include "twitchcode.h"
#include "stringvector.h"
//#include "ReadfromFile.h"

#include <sapi.h>
//#include <sphelper.h>
//#include <atlbase.h>

#pragma warning(disable : 4996)

const long long int buffer_size = 10000;

void initialize_tts(ISpVoice** pVoice)
{
	CoInitialize(NULL);
	CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)pVoice);
}

namespace Init
{
	SDL_Renderer *renderer = NULL;
	int screen_width = 1920;
	int screen_height = 1080;

	//Set background color on window

	void initialize_SDL()
	{
		SDL_Init(SDL_INIT_VIDEO);

		SDL_Window *window = SDL_CreateWindow("Twitch Overlay", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Init::screen_width, Init::screen_height, SDL_WINDOW_SHOWN);
		Init::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


		SDL_SetRenderDrawColor(Init::renderer, 0, 0, 0, 255);

		SDL_RenderClear(Init::renderer);
	}
}

int main(int argc, char **argv)
{
	//initialize tts
	ISpVoice * pVoice = NULL;
	initialize_tts(&pVoice);
	wchar_t wstr[buffer_size];

	long long currency = 100; //replace this with donation amount converted into pennies

	char* paid_tts = (char*)malloc(sizeof(char) * buffer_size);

	//initialize Twitch Message Copy Over
	unsigned int copy_n_count = 0;
	unsigned int copy_buffer = 1000; 
	char **copy_message = new char*[copy_buffer];

	//initialize SDL
	Init::initialize_SDL();
	srand(time(0));

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
	
	//https://stackoverflow.com/questions/3220477/how-to-use-clock-in-c examples

	int t1 = clock();
	int t2 = clock();
	double time_passed = (t2 - t1) / (double)CLOCKS_PER_SEC;

	SDL_Surface *tmp = IMG_Load("qt.jpg");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(Init::renderer, tmp);
	SDL_FreeSurface(tmp);

	SDL_Surface *font = IMG_Load("font_sheet.png"); //each char size is 49x46
	SDL_Texture *font_texture = SDL_CreateTextureFromSurface(Init::renderer, font);
	SDL_FreeSurface(font); //free surface since we pushed surface to texture

	float fancy_x = 0;
	float fancy_y = 0;
	
	bool runTime = true;

	printf("chat log\n");	
	for (;;)
	{
		srand(time(0));
		unsigned int timestamp = clock();

		unsigned int current_time = SDL_GetTicks();

		unsigned int last_text_change_tiem = SDL_GetTicks();

		//consume all window events first
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
			{
				break;
			}
		}
		
		//collect all messages from all channels
			//pretty sure ths is not how you spell COMMUNICATE
		Twitch::communicate(&incoming, &connection, timestamp);
		if (connection.active == false)
		{
			printf("connection was closed!\n");
			break;
		}

		//printf("number of messages: %d\n", incoming.n_count);

		//incoming.n_count = 1;
		//incoming.message[0] = "hello";

		//print received messages from all channels
		for (int i = 0; i < incoming.n_count; i++)
		{
			copy_message[copy_n_count] = (char*)malloc(sizeof(char) * copy_buffer);
			strcpy(copy_message[copy_n_count], incoming.message[i]);
			copy_n_count++;
			//retrieve messages from twitch
			//printf("%s@%s|(%.2f)->%s\n", incoming.username[i], incoming.channel[i], (double)timestamp / CLOCKS_PER_SEC, incoming.message[i]);

			int res = strcmp("guildude", incoming.username[i]); //replace my username with text of donater 
			
			//Note to self, make sure symbols don't deduct from currency
			if (res == 0)
			{
				//printf("%s", incoming.message[i]);
				/*cout << "currency: " << currency << endl;
				cout << "strlen: " << strlen(incoming.message[i]) - 1 << endl;
				currency -= (strlen(incoming.message[i]) - 1);
				cout << "currency: " << currency << endl;*/

				//pVoice->GetStatus(&status, NULL);

				if (currency < 0)
				{
					cout << "You have gone past your character limit" << endl; //replace this with an actual twitch message sent
				}
				else if (currency > 0)
				{
					//printf("%s", incoming.message[i]);
					for (int j = 0; j < currency; j++)
					{
						strcpy(paid_tts, incoming.message[i]);
					}
					//printf("%s", paid_tts);
					//cerr << currency << endl; //deduct 1 from strlen to get exact char value
					mbstowcs(wstr, incoming.message[i], buffer_size); //convert twitch messages into wide character
					pVoice->Speak(wstr, SVSFlagsAsync | SVSFPurgeBeforeSpeak, NULL); //output twitch messages 
				}
			}
			
		}

		SDL_Rect dest;
		dest.x = 0;
		dest.y = 0;
		dest.w = 56; //font size
		dest.h = 56;
		
		for (int i= 0; i < copy_n_count; i++)
		{
			//cout << copy_message[1] << endl;
			for (int j = 0; j < strlen(copy_message[i]); j++)
			{
				SDL_Rect src;

				src.x = 64 * (copy_message[i][j] % 16);
				src.y = 64 * (copy_message[i][j] / 16);
				src.w = 64; 
				src.h = 64;

				SDL_RenderCopyEx(Init::renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);
				dest.x += 56;

				if (dest.x >= 1920 - 64)
				{
					dest.x = 0;
					dest.y += 56;
				}
				if (dest.y >= 1080 - 64)
				{
					copy_n_count = 1;
					dest.x = 0;
					dest.y = 0;
				}
			}

			dest.y += 56;
			dest.x = 100;

		}
		
		

		/*
		SDL_Rect image_rect;
		image_rect.x = 600;
		image_rect.y = 600;
		image_rect.w = 60;
		image_rect.h = 60;

		SDL_Rect screen_rect;
		screen_rect.x = 50;
		screen_rect.y = 50;
		screen_rect.w = 240;
		screen_rect.h = 240;
		SDL_RenderCopyEx(Init::renderer, texture, &image_rect, &screen_rect, 0, NULL, SDL_FLIP_NONE);

		screen_rect.x = 300;
		screen_rect.y = 300;
		screen_rect.w = 60;
		screen_rect.h = 60;
		SDL_RenderCopyEx(Init::renderer, texture, &image_rect, &screen_rect, 0, NULL, SDL_FLIP_NONE);


		fancy_x += 0.01;
		fancy_y += 0.01;

		image_rect.x = fancy_x;
		image_rect.y = fancy_y;
		image_rect.w = 60;
		image_rect.h = 60;

		screen_rect.x = 300;
		screen_rect.y = 500;
		screen_rect.w = 60;
		screen_rect.h = 60;
		SDL_RenderCopyEx(Init::renderer, texture, &image_rect, &screen_rect, 0, NULL, SDL_FLIP_NONE);
		*/
		/*
		if (current_time - last_text_change_tiem > 1000) //1000 is 1 frame per second
		{
			last_text_change_tiem = current_time;

			for (int i = 0; i < 16; i++)
			{
				//incoming.message[i] = 'a' +
			}
		}*/

		SDL_RenderPresent(Init::renderer);
	}

	getchar();
	return 0;
}
