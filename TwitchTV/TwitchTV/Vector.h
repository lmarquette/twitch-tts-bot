#pragma once

struct Str_Vector
{
	char **words_array;
	int count;
	int array_size;
};

void init_Str_Vector(Str_Vector *strv)
{
	strv->array_size = 100;
	strv->count = 0;
	strv->words_array = new char *[strv->array_size];
	assert(strv->words_array);
}

void resize_Str_Vector(Str_Vector *strv)
{
	strv->array_size *= 1.5;

	char **tmp_array = new char *[strv->array_size];
	assert(tmp_array);

	for (int i = 0; i < strv->array_size; i++)
	{
		tmp_array[i] = strv->words_array[i];
	}

	delete[] strv->words_array;

	strv->words_array = tmp_array;
}

void add_Str_Vector(Str_Vector *strv, char *x)
{
	if (strv->count >= strv->array_size)
	{
		resize_Str_Vector(strv);
	}

	strv->words_array[strv->count] = x;
	strv->count++;
}

char *sort_Str_Vector(Str_Vector *strv)
{
	for (int j = 0; j < strv->count-1; j++)
	{
		char *tmp = new char[50];

		for (int i = j + 1; i < strv->count; i++)
		{
			if (strcmp(strv->words_array[j], strv->words_array[i]) > 0)
			{
				tmp = strv->words_array[j];
				strv->words_array[j] = strv->words_array[i];
				strv->words_array[i] = tmp;
			}
		}	
	}			
	return *strv->words_array;
}

