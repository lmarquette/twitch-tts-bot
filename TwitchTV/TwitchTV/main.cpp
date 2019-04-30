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


//Miku sprite sheet is 1225 x 1470px, 30 frames, 1 columns 30 rows
//https://www.piskelapp.com/ for gif to sprite

namespace Data
{
	SDL_Renderer *renderer = NULL;
	int screen_width = 1920;
	int screen_height = 1080;
	const int num_memes = 10;
	const int meme_buffer = 10000;
	const int array_buffer = 1000;
	SDL_Texture **meme_textures = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * meme_buffer);

	//parse array for these words
	char* memes[num_memes] = { "feelsbadman", "kappa", "kreygasm", "omegalul",
		"monkaS", "lul", "pogchamp", "poggers", 
		"residentsleeper", "wesmart" };

	//filenames
	char* meme_filenames[num_memes] = { "feelsbadman.png", "kappa.jpg", "kreygasm.png", 
		"omegalul.png", "monkaS.png", "lul.jpg", "pogchamp.jpg", "poggers.png", 
		"residentsleeper.png", "wesmart.png" };

	struct Meme_Data
	{
		float x, y;
		int w, h;
		float vel_x, vel_y;
		int meme_index;
		unsigned int creation_time;
	};
	//str = incoming message, compare = what you're looking for
	void ghetto_strstr(char* str)
	{
		static char tmp[4096];
		strcpy(tmp, str);
		int counter = 0;
		cout << "str: " << str << endl;
		cout << "compare: " << memes[3] << endl;
		
		for (int i = 0; i < num_memes; i++)
		{
			if (memes[i] == str)
			{
				cout << "yes" << endl;
				cout << memes[i] << endl;
				cout << str << endl;
			}
		}
	}

	void intialize_meme_data(Meme_Data *d)
	{
		*d = { 0 };
	}

	void initialize_tts(ISpVoice** pVoice)
	{
		CoInitialize(NULL);
		CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)pVoice);
	}

	void intialize_Memes()
	{
		for (int i = 0; i < num_memes; i++)
		{
			SDL_Surface *tmp = IMG_Load(meme_filenames[i]);
			meme_textures[i] = SDL_CreateTextureFromSurface(renderer, tmp);
			if (meme_textures[i] == NULL) cout << meme_filenames[i] << " failed to load" << endl;
			SDL_FreeSurface(tmp);
		}
	}

	void initialize_SDL()
	{
		SDL_Init(SDL_INIT_VIDEO);

		SDL_Window *window = SDL_CreateWindow("Twitch Overlay", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Data::screen_width, Data::screen_height, SDL_WINDOW_SHOWN);
		Data::renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		//Set background color on window
		//Set color to what the screen is ignoring 
		SDL_SetRenderDrawColor(Data::renderer, 0, 255, 0, 255); 
		SDL_RenderClear(Data::renderer);

	}

	void Draw(Meme_Data *d, int index)
	{
		SDL_Rect screen_pos;
		screen_pos.x = d[index].x;
		screen_pos.y = d[index].y;
		screen_pos.w = d[index].w;
		screen_pos.h = d[index].h;

		SDL_RenderCopyEx(renderer, meme_textures[d[index].meme_index], NULL, &screen_pos, 0, NULL, SDL_FLIP_NONE);
		
	}

	int createactor(unsigned char *arr, int array_size)
	{
		for (int i = 0; i < array_size; i++)
		{
			if (arr[i] == 0)
			{
				arr[i] = 1;
				return i;
			}
		}
	}

	void destroyactor(unsigned char* active, int index)
	{
		active[index] = 0;
	}

	int parse_string(char* incoming_message)
	{
		static char lowercase[4096];
		strcpy(lowercase, incoming_message);
		char *compare;
	
		for (int i = 0; i < strlen(incoming_message); i++)
		{
			lowercase[i] = tolower(incoming_message[i]);
		}
		
		for (int i = 0; i < num_memes; i++)
		{
			compare = strstr(lowercase, memes[i]);
			if (compare != NULL) return i;
		}
		return -1;
	}
}


int main(int argc, char **argv)
{
	//initialize tts
	ISpVoice * pVoice = NULL;
	Data::initialize_tts(&pVoice);
	wchar_t wstr[buffer_size];

	long long currency = 100; //replace this with donation amount converted into pennies

	char* paid_tts = (char*)malloc(sizeof(char) * buffer_size);

	//initialize Twitch Message Copy Over
	unsigned int copy_n_count = 0;
	unsigned int current_copy_index = 0;
	unsigned int number_of_copies_to_show = 7;

	unsigned int copy_buffer = 1000; 
	char **copy_message = new char*[copy_buffer];
	char **copy_username = new char*[copy_buffer];

	long int *y_pos = new long int[1000];


	char* miku_gif = new char[30];
	for (int i = 0; i < 30; i++)
	{
		miku_gif[i] = (char)malloc(sizeof(char) * 30);
	}

	//figure out how to remove this
	for (int i = 0; i < number_of_copies_to_show; i++)
	{
		copy_username[i] = "me";
		copy_message[i] = "hello";
	}
	copy_n_count = number_of_copies_to_show;

	int meme_array_size = 10000;
	//Initialize Meme Data 
	Data::Meme_Data *meme_data = new Data::Meme_Data[meme_array_size];
	unsigned char *active = new unsigned char[meme_array_size];
	//intialize array to 0
	for (int i = 0; i < meme_array_size; i++)
	{
		active[i] = 0;
		Data::intialize_meme_data(&meme_data[i]);
	}

	//initialize SDL
	Data::initialize_SDL();
	srand(time(0));

	//initialize Memes
	Data::intialize_Memes();

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
	
	SDL_Surface *font = IMG_Load("font_sheet.png");
	SDL_Texture *font_texture = SDL_CreateTextureFromSurface(Data::renderer, font);
	SDL_FreeSurface(font); //free surface since we pushed surface to texture

	SDL_Surface *miku = IMG_Load("miku.png");
	SDL_Texture *miku_texture = SDL_CreateTextureFromSurface(Data::renderer, miku);
	SDL_FreeSurface(font);

	float fancy_x = 0;
	float fancy_y = 0;
	
	bool runTime = true;

	printf("chat log\n");	
	for (;;)
	{
		srand(time(0));
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
		Twitch::communicate(&incoming, &connection, current_time);
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
			copy_username[copy_n_count] = (char*)malloc(sizeof(char) * copy_buffer);
			strcpy(copy_username[copy_n_count], incoming.username[i]);
			strcpy(copy_message[copy_n_count], incoming.message[i]);
			copy_n_count++;
			current_copy_index++;
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

			//parse messages and grabs which index the meme is at in the array
			int parsed_index = Data::parse_string(incoming.message[i]);
			if (parsed_index != -1)
			{
				Data::ghetto_strstr(incoming.message[i]);
				for (int yolo = 0; yolo < rand() % 100; yolo++)//spawn multiple emotes
				{
					int k = Data::createactor(active, meme_array_size);
					if (k != -1)
					{
						meme_data[k].w = rand() % 300 + 50;
						meme_data[k].h = rand() % 300 + 50;
						meme_data[k].x = Data::screen_width / 2 - meme_data[k].w / 2;
						meme_data[k].y = Data::screen_height / 2 - meme_data[k].h / 2;
						meme_data[k].vel_x = 1.0 - 2.0*rand() / RAND_MAX;
						meme_data[k].vel_y = 1.0 - 2.0*rand() / RAND_MAX;
						meme_data[k].creation_time = current_time;
						meme_data[k].meme_index = parsed_index;
					} 
					else
					{
						printf("could not find inactive\n");
					}
				}
			}
		}
		
		//destroy actor
		for (int i = 0; i < meme_array_size; i++)
		{
			if (active[i] == 0) continue;
			if (current_time - meme_data[i].creation_time > 10000)
			{
				Data::destroyactor(active, i);
			}
		}

		SDL_RenderClear(Data::renderer);

		SDL_Rect dest;
		dest.x = 0;
		dest.y = 0;
		dest.w = 56; //font size
		dest.h = 56;

		SDL_Rect miku_dest;
		dest.x = 400;
		dest.y = 400;
		dest.w = 600; //font size
		dest.h = 600;

		SDL_Rect miku_src;
		for (int i = 0; i < 30; i++)
		{
			cout << "miku" << endl;
			miku_src.x = 245 * (miku_gif[i] % 1); //column
			miku_src.y = 245 * (miku_gif[i] / 30); //row
			miku_src.w = 245;
			miku_src.h = 245;
			SDL_RenderCopyEx(Data::renderer, miku_texture, &miku_src, &miku_dest, 0, NULL, SDL_FLIP_NONE);
		}

		//hard cap how many characters can scroll across the screen
		//Render incoming.message[i] with font on the screen for users to see
		for (int i = current_copy_index; i < current_copy_index+number_of_copies_to_show; i++)
		{
			//output username
			SDL_Rect src;
			for (int j = 0; j < strlen(copy_username[i]); j++)
			{
				src.x = 64 * (copy_username[i][j] % 16); //column
				src.y = 64 * (copy_username[i][j] / 16); //row
				src.w = 64;
				src.h = 64;
				SDL_RenderCopyEx(Data::renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

				dest.x += 40;
			}

			src.x = 64 * 10;
			src.y = 64 * 3;
			SDL_RenderCopyEx(Data::renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

			dest.x += 60;
			//output their message
			for (int j = 0; j < strlen(copy_message[i]); j++)
			{
				src.x = 64 * (copy_message[i][j] % 16);
				src.y = 64 * (copy_message[i][j] / 16);
	
				SDL_RenderCopyEx(Data::renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

				dest.x += 40;

				if (dest.x >= Data::screen_width - 64)
				{
					dest.x = 0;
					dest.y += 56;
				}
				if (dest.y >= Data::screen_height - 64)
				{
					dest.x = 0;
					dest.y = 0;
				}
			}
			dest.y += 56; //new line
			dest.x = 0; //set font back to beginning
		}

		//render memes on the page
		for (int i = 0; i < meme_array_size; i++)
		{
			if (active[i] == 0) continue;
			meme_data[i].x += meme_data[i].vel_x;
			meme_data[i].y += meme_data[i].vel_y;

			if (meme_data[i].x <= 0)
			{
				meme_data[i].vel_x *= -1;
			}
			if (meme_data[i].x + meme_data[i].w >= Data::screen_width)
			{
				meme_data[i].vel_x *= -1;
			}
			if (meme_data[i].y <= 0)
			{
				meme_data[i].vel_y *= -1;
			}
			if (meme_data[i].y + meme_data[i].h >= Data::screen_height)
			{
				meme_data[i].vel_y *= -1;
			}

			Data::Draw(meme_data, i);
		}

		SDL_RenderPresent(Data::renderer);
	}
	
	getchar();
	return 0;
}
