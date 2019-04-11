#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#include<iostream>
#include<assert.h>
#include<time.h>

using namespace std;

#include "Vector.h"


void main()
{
	Str_Vector words_list;

	char *sentence = "hello! world, this is me.waqas, how are you?";

	init_Str_Vector(&words_list);

	char *delimiters = " .,!\n\r?";
	char tmp_msg[256];

	strcpy(tmp_msg, sentence);
	char *word = strtok(tmp_msg, delimiters);

	for (;;)
	{
		if (word == NULL) break;
		add_Str_Vector(&words_list, word);

		word = strtok(NULL, delimiters);
	}

	sort_Str_Vector(&words_list);

	for (int i = 0; i < words_list.count; i++)
	{
		cout << words_list.words_array[i] << "\n";
	}

	system("Pause");
}