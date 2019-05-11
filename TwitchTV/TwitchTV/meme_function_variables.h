#pragma once

#include <iostream>
#include <stdlib.h>
#include "meme_data.h"

#include "SDL2-2.0.9/include/SDL.h"
#include "SDL2-2.0.9/include/SDL_image.h"

#include "SDL2-2.0.9\include\SDL.h"
#include "SDL2-2.0.9\include\SDL_image.h"

#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2.lib")
#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2main.lib")
#pragma comment(lib,"SDL2-2.0.9\\lib\\x86\\SDL2_image.lib")

#pragma comment(linker,"/subsystem:console")

#pragma warning(disable : 4996)

SDL_Renderer *renderer = NULL;
int screen_width = 1920;
int screen_height = 1080;
const int meme_buffer_size = 10000;
const int gif_buffer_size = 1000000;
const int frame_buffer_size = 10000;
unsigned int *last_gif_frame_updated;

SDL_Texture **meme_textures = NULL;
SDL_Texture **gif_textures = NULL;