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

#include "meme_functions.h"

using namespace std;

#include "twitchcode.h"
#include "stringvector.h"

#include <sapi.h>


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
		copy_username[i] = "//";
		copy_message[i] = "intializing";
	}

	copy_n_count = number_of_copies_to_show;

	int meme_array_size = 10000;
	int gif_array_size = 5000;

	//Initialize Meme Data 
	Data::meme_data *meme_data = new Data::meme_data[meme_array_size];
	Data::gif_data *gif_data = new Data::gif_data[gif_array_size];
	unsigned char *active_meme = new unsigned char[meme_array_size];
	unsigned char *active_gif = new unsigned char[gif_array_size];

	//intialize meme array to 0
	for (int i = 0; i < meme_array_size; i++)
	{
		active_meme[i] = 0;
		Data::intialize_meme_data(&meme_data[i]);
	}

	//initialize gif array to 0
	for (int i = 0; i < gif_array_size; i++)
	{
		active_gif[i] = 0;
		Data::intialize_gif_data(&gif_data[i]);
	}

	int parsed_index_memes = -1;
	int parsed_index_gifs = -1;

	//initialize
	Data::initialize_sdl();
	Data::intialize_memes();
	Data::initialize_gifs();
	Data::intialize_gif_frame_update();
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
	SDL_Texture *font_texture = SDL_CreateTextureFromSurface(renderer, font);
	SDL_FreeSurface(font); //free surface since we pushed surface to texture

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
				return 0;
			}
		}

		Twitch::communicate(&incoming, &connection, current_time);
		if (connection.active == false)
		{
			printf("connection was closed!\n");
			break;
		}

		//print received messages from all channels
		for (int i = 0; i < incoming.n_count; i++)
		{
			//retrieve/copy messages from twitch
			copy_message[copy_n_count] = (char*)malloc(sizeof(char) * copy_buffer);
			copy_username[copy_n_count] = (char*)malloc(sizeof(char) * copy_buffer);
			strcpy(copy_username[copy_n_count], incoming.username[i]);
			strcpy(copy_message[copy_n_count], incoming.message[i]);
			copy_n_count++;
			current_copy_index++;

			//parse messages and grabs which index the meme is at in the array
			parsed_index_memes = Data::parse_string_memes(incoming.message[i]);
			parsed_index_gifs = Data::parse_string_gif(incoming.message[i]);

			//create meme
			if (parsed_index_memes != -1)
			{
				for (int yolo = 0; yolo < rand() % 100; yolo++)//spawn multiple emotes
				{
					int k = Data::create_actor_memes(active_meme, meme_array_size);
					if (k != -1)
					{
						meme_data[k].w = rand() % 100 + 50;
						meme_data[k].h = rand() % 100 + 50;
						meme_data[k].x = screen_width / 2 - meme_data[k].w / 2;
						meme_data[k].y = screen_height / 2 - meme_data[k].h / 2;
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
			
			//create gif
			if (parsed_index_gifs != -1)
			{
				int k = Data::create_actor_gifs(active_gif, gif_array_size);
				if (k != -1)
				{
					cout << gif_width[parsed_index_gifs] << endl;
					gif_data[k].w = gif_width[parsed_index_gifs];
					gif_data[k].h = gif_height[parsed_index_gifs];
					gif_data[k].x = screen_width / 2 - gif_data[k].w / 2;
					gif_data[k].y = screen_height / 2;
					//gif_data[k].y = Data::screen_height + gif_height[parsed_index_gifs];
					gif_data[k].creation_time = current_time;
					gif_data[k].gif_index = parsed_index_gifs;

					cout << "parsed gif: " << parsed_index_gifs << endl;
					cout << "What index: " << k << endl;
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
		
		//destroy meme
		for (int i = 0; i < meme_array_size; i++)
		{
			if (active_meme[i] == 0) continue;
			if (current_time - meme_data[i].creation_time > 4000)
			{
				Data::destroy_actor(active_meme, i);
			}
		}

		//destroy gif
		for (int i = 0; i < gif_array_size; i++)
		{
			if (active_gif[i] == 0) continue;
			if (current_time - gif_data[i].creation_time > 10000)
			{
				Data::destroy_actor(active_gif, i);
			}
		}

		SDL_RenderClear(renderer);

		double chatbox_x = screen_width / 2;
		double chatbox_y = screen_height / 2 + 400;
		double chat_start_pos_x = 0;
		double chat_start_pos_y = screen_height / 2;

		SDL_Rect dest;
		dest.x = chat_start_pos_x;
		dest.y = chat_start_pos_y;
		dest.w = 30; //font size
		dest.h = 30;

		//Render incoming.message[i] with font on the screen for users to see
		//current_time = SDL_GetTicks();
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
					SDL_RenderCopyEx(renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

					dest.x += 35; //spacers between characters
				}

				//to render colon from font sheet
				src.x = 64 * 10;
				src.y = 64 * 3;
				SDL_RenderCopyEx(renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

				dest.x += 50; //spacer
				//output their message
				for (int j = 0; j < strlen(copy_message[i]); j++)
				{
					src.x = 64 * (copy_message[i][j] % 16);
					src.y = 64 * (copy_message[i][j] / 16);

					SDL_RenderCopyEx(renderer, font_texture, &src, &dest, 0, NULL, SDL_FLIP_NONE);

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
		for (int i = 0; i < meme_array_size; i++)
		{
			if (active_meme[i] == 0) continue;
			meme_data[i].x += meme_data[i].vel_x;
			meme_data[i].y += meme_data[i].vel_y;

			//boundaries for memes to bounce off of
			if (meme_data[i].x <= 0)
			{
				meme_data[i].vel_x *= -1;
			}
			if (meme_data[i].x + meme_data[i].w >= screen_width)
			{
				meme_data[i].vel_x *= -1;
			}
			if (meme_data[i].y <= 0)
			{
				meme_data[i].vel_y *= -1;
			}
			if (meme_data[i].y + meme_data[i].h >= screen_height)
			{
				meme_data[i].vel_y *= -1;
			}
			//draw le meme
			Data::draw_meme(meme_data, i);
		}

		
		current_time = SDL_GetTicks();
		for (int i = 0; i < gif_array_size; i++)
		{
			if (active_gif[i] == 0) continue;
			//update gif frame
			if (current_time - last_gif_frame_updated[i] > 50)
			{
				last_gif_frame_updated[i] = current_time;
				//current_frame++;
				//cout << "draw gif index: " << gif_data[i].gif_index << endl;
				//if current_frame > total frames set current frame to 0
				gif_data[i].current_frame++;
				if (gif_data[i].current_frame >= gif_height[gif_data[i].gif_index]) gif_data[i].current_frame = 0;
			}
			draw_gif(gif_data, i);
		}

		SDL_RenderPresent(renderer);
	}
	
	getchar();
	return 0;
}
