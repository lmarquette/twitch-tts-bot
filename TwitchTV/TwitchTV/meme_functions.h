#pragma once
#include "meme_data.h"
#include "meme_function_variables.h"

namespace Data
{
	struct meme_data
	{
		float x, y;
		int w, h;
		float vel_x, vel_y;
		int meme_index;
		unsigned int creation_time;
	};

	struct gif_data
	{
		float x, y;
		int w, h;
		float vel_x, vel_y;
		int gif_index;
		unsigned int creation_time;
		int current_frame;
		SDL_Rect gif_src;
	};

	struct particle_system
	{
		int r, g, b;
		float x, y;
		int w, h;
		float vel_x, vel_y;
		double gravity;
		unsigned int creation_time;
	};

	void initialize_meme_data(meme_data *d)
	{
		*d = { 0 };
	}

	void initialize_gif_data(gif_data *d)
	{
		*d = { 0 };
	}

	void initiailize_particle_system(particle_system *d)
	{
		d->r = 255;
		d->g = 0;
		d->b = 0;
		d->w = 10;
		d->h = 10;
		d->vel_x = 0;
		d->vel_y = 0;
		d->gravity = .1;
		d->x = screen_width / 2;
		d->y = screen_height / 2;
	}

	void initialize_sdl()
	{
		SDL_Init(SDL_INIT_VIDEO);

		SDL_Window *window = SDL_CreateWindow("Twitch Overlay", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	}

	void initialize_memes()
	{
		meme_textures = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * meme_buffer_size);
		for (int i = 0; i < num_memes; i++)
		{
			SDL_Surface *tmp_meme = IMG_Load(meme_filenames[i]);
			meme_textures[i] = SDL_CreateTextureFromSurface(renderer, tmp_meme);
			if (meme_textures[i] == NULL) cout << meme_filenames[i] << " failed to load" << endl;
			SDL_FreeSurface(tmp_meme);
		}
	}

	void initialize_gifs()
	{
		gif_textures = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * gif_buffer_size);
		for (int i = 0; i < num_gifs; i++)
		{
			SDL_Surface *tmp_gif = IMG_Load(gif_filenames[i]);
			gif_textures[i] = SDL_CreateTextureFromSurface(renderer, tmp_gif);
			if (gif_textures[i] == NULL) cout << gif_filenames[i] << " failed to load" << endl;
			SDL_FreeSurface(tmp_gif);
		}
	}

	void initialize_gif_frame_update()
	{
		last_gif_frame_updated = (unsigned int*)malloc(sizeof(unsigned int) * frame_buffer_size);

		for (int i = 0; i < frame_buffer_size; i++)
		{
			last_gif_frame_updated[i] = 0;
		}
	}

	void background_color()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
	}

	void draw_particles(particle_system *d, int index)
	{	
		SDL_Rect rect;
		rect.x = d[index].x;
		rect.y = d[index].y;
		rect.w = d[index].w;
		rect.h = d[index].h;

		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderFillRect(renderer,&rect);
	}

	void draw_meme(meme_data *d, int index)
	{
		SDL_Rect screen_pos;
		screen_pos.x = d[index].x;
		screen_pos.y = d[index].y;
		screen_pos.w = d[index].w;
		screen_pos.h = d[index].h;

		SDL_RenderCopyEx(renderer, meme_textures[d[index].meme_index], NULL, &screen_pos, 0, NULL, SDL_FLIP_NONE);
	}

	void draw_gif(gif_data *d, int index)
	{
		SDL_Rect screen_pos;
		screen_pos.x = d[index].x;
		screen_pos.y = d[index].y;
		screen_pos.w = d[index].w;
		screen_pos.h = d[index].h;

		d[index].gif_src.y = d[index].current_frame * gif_height[d[index].gif_index];
		d[index].gif_src.w = gif_width[d[index].gif_index];
		d[index].gif_src.h = gif_height[d[index].gif_index];
		SDL_RenderCopyEx(renderer, gif_textures[d[index].gif_index], &d[index].gif_src, &screen_pos, 0, NULL, SDL_FLIP_NONE);
	}

	int create_actor_meme(unsigned char *arr, int array_size)
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

	int create_actor_gif(unsigned char *arr, int array_size)
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

	int create_actor_particle(unsigned char *arr, int array_size)
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

	void destroy_actor(unsigned char* active, int index)
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