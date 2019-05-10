#pragma once
#include <iostream>
#include <stdlib.h>
#include "Memes.h"

#include "SDL2-2.0.9/include/SDL.h"
#include "SDL2-2.0.9/include/SDL_image.h"

#include "SDL2-2.0.9\include\SDL.h"
#include "SDL2-2.0.9\include\SDL_image.h"

#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2main.lib")
//copy the SDL2_image.lib from the SDL imaege folder to your SDL lib folder
#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2_image.lib")

#pragma comment(linker,"/subsystem:console")

#pragma warning(disable : 4996)

namespace Data
{
	SDL_Renderer *renderer = NULL;
	int screen_width = 1920;
	int screen_height = 1080;
	const int meme_buffer = 10000;
	const int gif_buffer = 1000000;
	const int array_buffer = 1000;
	//SDL_Texture **tmp = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * 10000);
	SDL_Texture **meme_textures = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * meme_buffer);
	SDL_Texture **gif_textures = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * gif_buffer);


	void initialize_SDL()
	{
		SDL_Init(SDL_INIT_VIDEO);

		SDL_Window *window = SDL_CreateWindow("Twitch Overlay", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Data::screen_width, Data::screen_height, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

		//Set background color on window
		SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
		SDL_RenderClear(renderer);

	}

	struct Meme_Data
	{
		float x, y;
		int w, h;
		float vel_x, vel_y;
		int meme_index;
		int gif_index;
		unsigned int creation_time;
	};

	void intialize_meme_data(Meme_Data *d)
	{
		*d = { 0 };
	}

	void intialize_Memes()
	{
		for (int i = 0; i < num_memes; i++)
		{
			SDL_Surface *tmp_meme = IMG_Load(meme_filenames[i]);
			meme_textures[i] = SDL_CreateTextureFromSurface(renderer, tmp_meme);
			if (meme_textures[i] == NULL) cout << meme_filenames[i] << " failed to load" << endl;
			SDL_FreeSurface(tmp_meme);
		}
	}

	void initialize_Gifs()
	{
		for (int i = 0; i < num_gifs; i++)
		{
			SDL_Surface *tmp_gif = IMG_Load(gif_filenames[i]);
			gif_textures[i] = SDL_CreateTextureFromSurface(renderer, tmp_gif);
			if (gif_textures[i] == NULL) cout << gif_filenames[i] << " failed to load" << endl;
			SDL_FreeSurface(tmp_gif);
		}
	}

	void Draw_Memes(Meme_Data *d, int index)
	{
		SDL_Rect screen_pos;
		screen_pos.x = d[index].x;
		screen_pos.y = d[index].y;
		screen_pos.w = d[index].w;
		screen_pos.h = d[index].h;

		SDL_RenderCopyEx(renderer, meme_textures[d[index].meme_index], NULL, &screen_pos, 0, NULL, SDL_FLIP_NONE);
	}

	void Draw_Gifs(Meme_Data *d, int index)
	{
		SDL_Rect gif_src;
		gif_src.x = d[index].x;
		gif_src.y = d[index].y;
		gif_src.w = d[index].w;
		gif_src.h = d[index].h;
	
		SDL_RenderCopyEx(renderer, gif_textures[d[index].gif_index], NULL, &gif_src, 0, NULL, SDL_FLIP_NONE);

		gif_src.y += gif_height[index];
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

	int parse_string_gif(char* incoming_message)
	{
		static char lowercase[4096];
		strcpy(lowercase, incoming_message);
		char *compare;

		//convert incoming messages to lowercase
		for (int i = 0; i < strlen(incoming_message); i++)
		{
			lowercase[i] = tolower(incoming_message[i]);
		}

		//browse through gif array to find appropriate word
		for (int i = 0; i < num_gifs; i++)
		{
			compare = strstr(lowercase, gifs[i]);
			if (compare != NULL) return i;
		}
		return -1;
	}

	int parse_string_memes(char* incoming_message)
	{
		static char lowercase[4096];
		strcpy(lowercase, incoming_message);
		char *compare;

		//convert incoming messages to lowercase
		for (int i = 0; i < strlen(incoming_message); i++)
		{
			lowercase[i] = tolower(incoming_message[i]);
		}

		//browse through meme array to find appropriate word
		for (int i = 0; i < num_memes; i++)
		{
			compare = strstr(lowercase, memes[i]);
			if (compare != NULL) return i;
		}
		return -1;
	}
}