#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
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
	int screen_width = 800;
	int screen_height = 600;
}

int main(int argc, char **argv)
{
	//initialize tts
	ISpVoice * pVoice = NULL;
	initialize_tts(&pVoice);
	wchar_t wstr[buffer_size];

	char copy_string[buffer_size];
	
	long long currency = 100; //replace this with donation amount converted into pennies

	char* paid_tts = (char*)malloc(sizeof(char) * buffer_size);

	//initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
	srand(time(0));

	SDL_Window *window = SDL_CreateWindow("Twitch Overlay", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Init::screen_width, Init::screen_height, SDL_WINDOW_SHOWN);
	Init::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

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

		//print received messages from all channels
		for (int i = 0; i < incoming.n_count; i++)
		{
			/*printf("%s@%s|(%.2f)->%s\n", incoming.username[i], incoming.channel[i], (double)timestamp / CLOCKS_PER_SEC, incoming.message[i]);
			//printf("message length: %d\n", strlen(incoming.message[i]));
			//if (strcmp("!test\r", incoming.message[i]) == 0)
			{
				Twitch::send_Message(&connection, "guildude", "test received");
				Sleep(1000);
			}
			
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
				sprintf(tmp, "Hello @%s this is leo%d", incoming.username[i], rand() % 256); 

				Twitch::send_Message(&connection, "guildude", tmp);
				//cout << "Time: " << time << endl;

			}
			//cout << "Time: " << time_passed << endl;

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
					printf("%s", incoming.message[i]);
					for (int j = 0; j < currency; j++)
					{
						strcpy(paid_tts, incoming.message[i]);
					}
					printf("%s", paid_tts);
					//cerr << currency << endl; //deduct 1 from strlen to get exact char value
					mbstowcs(wstr, incoming.message[i], buffer_size); //convert twitch messages into wide character
					pVoice->Speak(wstr, SVSFlagsAsync | SVSFPurgeBeforeSpeak, NULL); //output twitch messages 
				}
			}
			else
			{
				cout << "Failed";
			}

			for (int i = 0; i < 500; i++)
			{
				SDL_Rect src;
				src.x = 64 * (incoming.message[i] % 16);
				src.y = 64 * (incoming.message[i] / 16);
				src.w = 64;
				src.h = 64;

				SDL_Rect dest;
				dest.x = 0;
				dest.y = 0;
				dest.w = 56; //font size
				dest.h = 56;

				SDL_RenderCopyEx(Init::renderer, texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);
				dest.x += 56;
			}

			//tokenize messages
			char *delimiters = " .,!\n\r?";
			char tmp_msg[512];

			strcpy(tmp_msg, incoming.message[i]);
			char *token = strtok(tmp_msg, delimiters);

			while (token != NULL)
			{
				//printf("%s\n", token);

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



		//Set background color on window
		SDL_SetRenderDrawColor(Init::renderer, 0, 0, 0, 255);

		SDL_RenderClear(Init::renderer);

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
