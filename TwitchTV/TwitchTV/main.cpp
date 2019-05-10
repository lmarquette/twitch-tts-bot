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

#include "Memes_Data.h"

using namespace std;

#include "twitchcode.h"
#include "stringvector.h"
//#include "ReadfromFile.h"

#include <sapi.h>
//#include <sphelper.h>
//#include <atlbase.h>

const long long int buffer_size = 10000;

void initialize_tts(ISpVoice** pVoice)
{
	CoInitialize(NULL);
	CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void**)pVoice);
}

int main(int argc, char **argv)
{
	unsigned int start_time = clock();
	unsigned int last_time_speech = SDL_GetTicks();

	//initialize tts
	ISpVoice * pVoice = NULL;
	initialize_tts(&pVoice);
	wchar_t wstr[buffer_size];

	//initialize Twitch Message Copy Over
	unsigned int copy_n_count = 0;
	unsigned int current_copy_index = 0;
	unsigned int number_of_copies_to_show = 3;

	unsigned int copy_buffer = 1000; 
	char **copy_message = new char*[copy_buffer];
	char **copy_username = new char*[copy_buffer];

	//figure out how to remove this
	for (int i = 0; i < number_of_copies_to_show; i++)
	{
		copy_username[i] = "me";
		copy_message[i] = "hello";
	}

	copy_n_count = number_of_copies_to_show;

	int array_size = 10000;

	//Initialize Meme Data 
	Data::Meme_Data *meme_data = new Data::Meme_Data[array_size];
	unsigned char *active = new unsigned char[array_size];

	//intialize array to 0
	for (int i = 0; i < array_size; i++)
	{
		active[i] = 0;
		Data::intialize_meme_data(&meme_data[i]);
	}

	int parsed_index_memes = -1;
	int parsed_index_gifs = -1;

	//initialize
	Data::initialize_SDL();
	Data::intialize_Memes();
	Data::initialize_Gifs();
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
	
	SDL_Surface *font = IMG_Load("Font/font_sheet.png");
	SDL_Texture *font_texture = SDL_CreateTextureFromSurface(Data::renderer, font);
	SDL_FreeSurface(font); //free surface since we pushed surface to texture

	bool runTime = true;

	/*
	SDL_Rect miku_src = { 0,0,245,245 };
	SDL_Rect miku_dest;
	miku_dest.x = 400;
	miku_dest.y = 400;
	miku_dest.w = 600; //gif size
	miku_dest.h = 600;*/

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

			//parse messages and grabs which index the meme is at in the array
			parsed_index_memes = Data::parse_string_memes(incoming.message[i]);
			parsed_index_gifs = Data::parse_string_gif(incoming.message[i]);

			//memes
			if (parsed_index_memes != -1)
			{
				for (int yolo = 0; yolo < rand() % 100; yolo++)//spawn multiple emotes
				{
					int k = Data::createactor(active, array_size);
					if (k != -1)
					{
						meme_data[k].w = rand() % 100 + 50;
						meme_data[k].h = rand() % 100 + 50;
						meme_data[k].x = Data::screen_width / 2 - meme_data[k].w / 2;
						meme_data[k].y = Data::screen_height / 2 - meme_data[k].h / 2;
						meme_data[k].vel_x = .15*rand() / RAND_MAX;
						meme_data[k].vel_y = .15*rand() / RAND_MAX;
						meme_data[k].creation_time = current_time;
						meme_data[k].meme_index = parsed_index_memes;
					}
					else
					{
						printf("could not find inactive\n");
					}
				}
			}
			
			//gifs
			if (parsed_index_gifs != -1)
			{
				int k = Data::createactor(active, array_size);
				cout << k << endl;
				if (k != -1)
				{
					cout << parsed_index_gifs << endl;
					meme_data[k].w = gif_width[parsed_index_gifs];
					meme_data[k].h = gif_height[parsed_index_gifs];
					meme_data[k].x = Data::screen_width / 2 - meme_data[k].w / 2;
					meme_data[k].y = Data::screen_height + gif_height[parsed_index_gifs];
					meme_data[k].creation_time = current_time;
					meme_data[k].meme_index = parsed_index_gifs;
				}
				else
				{
					printf("could not find inactive\n");
				}
			}
		}

		//tts
		current_time = SDL_GetTicks();
		for (int i = 0; i < incoming.n_count; i++)
		{
			parsed_index_memes = Data::parse_string_memes(incoming.message[i]);
			if (parsed_index_memes != -1)
			{
					last_time_speech = current_time;
					mbstowcs(wstr, memes[parsed_index_memes], buffer_size); //convert twitch messages into wide character
					pVoice->Speak(wstr, SVSFlagsAsync, NULL); //output twitch messages
					break;
			}
		}
		
		//destroy actor
		for (int i = 0; i < array_size; i++)
		{
			if (active[i] == 0) continue;
			if (current_time - meme_data[i].creation_time > 6000)
			{
				Data::destroyactor(active, i);
			}
		}

		SDL_RenderClear(Data::renderer);

		double chatbox_x = Data::screen_width / 2;
		double chatbox_y = Data::screen_height / 2 + 400;
		double chat_start_pos_x = 0;
		double chat_start_pos_y = Data::screen_height / 2;

		SDL_Rect dest;
		dest.x = chat_start_pos_x;
		dest.y = chat_start_pos_y;
		dest.w = 30; //font size
		dest.h = 30;

		//hard cap how many characters can scroll across the screen
		//Render incoming.message[i] with font on the screen for users to see
		current_time = clock();
		if (current_time - start_time > 100)
		{
			for (int i = current_copy_index; i < current_copy_index + number_of_copies_to_show; i++)
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

					dest.x += 35; //spacers between characters
				}

				//to render colon from font sheet
				src.x = 64 * 10;
				src.y = 64 * 3;
				SDL_RenderCopyEx(Data::renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

				dest.x += 50; //spacer
				//output their message
				for (int j = 0; j < strlen(copy_message[i]); j++)
				{
					src.x = 64 * (copy_message[i][j] % 16);
					src.y = 64 * (copy_message[i][j] / 16);

					SDL_RenderCopyEx(Data::renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

					dest.x += 35; //gap between characters

					//chat window size
					if (dest.x >= chatbox_x) //checks if text has gone to 1920/2
					{
						dest.x = 0;
						dest.y += 40; //newline
					}
					if (dest.y >= chatbox_y)
					{
						//reset position
						dest.x = chat_start_pos_x;
						dest.y = chat_start_pos_y;
					}
				}
				dest.y += 40; //new line
				dest.x = 0; //set font back to beginning
			}
		}

		//render memes on the page
		for (int i = 0; i < array_size; i++)
		{
			if (active[i] == 0) continue;
			meme_data[i].x += meme_data[i].vel_x;
			meme_data[i].y += meme_data[i].vel_y;

			//boundaries for memes to bounce off of
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
			//draw le meme
			Data::Draw_Memes(meme_data, i);
		}

		//handle gif processing here
		current_time = clock();
		/*
		if (current_time-start_time >100)
		{
				//cout << "miku" << endl;
				//miku_src.x = 0; //column
				//miku_src.y = (miku_src.y + 245) % 7350; //row
				start_time = current_time;
				//SDL_RenderCopyEx(Data::renderer, miku_texture, &miku_src, &miku_dest, 0, NULL, SDL_FLIP_NONE);
		}
		*/
		//SDL_RenderCopyEx(Data::renderer, miku_texture, &miku_src, &miku_dest, 0, NULL, SDL_FLIP_NONE);

		SDL_RenderPresent(Data::renderer);
	}
	
	getchar();
	return 0;
}
