#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif 

#include <iostream>
#include <fstream>


namespace ary
{
	struct Vec
	{
		int n_count;
		int array_size;
		char **str;
		int **freq;
	};

	void init(Vec *v)
	{
		v->array_size = 50;
		v->n_count = 0;
		v->str = new char*[v->array_size];
	}

	void resize(Vec *v)
	{
		v->array_size *= 1.2;
		char **tmp = new char*[v->array_size];
		for (int i = 0; i < v->n_count; i++)
		{
			tmp[i] = v->str[i];
		}
		delete[] v->str;
		v->str = tmp;
	}

	void add(Vec *v, char *x)
	{
		if (v->n_count >= v->array_size)
		{
			resize(v);
		}

		v->str[v->n_count] = x;
		v->n_count++;
	}

	void sort(Vec *v)
	{
		for (int i = 0; i < v->n_count; i++)
		{
			for (int j = 0; j < v->n_count + 1; j++)
			{
				if (v->str[i] >= v->str[j])
				{
					char* tmp1 = v->str[i];
					char* tmp2 = v->str[j];
					v->str[i] = tmp2;
					v->str[j] = tmp1;
					//can also swap(i,j)
				}
			}
		}
	}
}

void main()
{
	char *test = "does this work correctly";
	char *delimeters = " .,\n\r!?";
	char msg[256];
	
	char* word = strtok(msg, delimeters);
	
	for(;;)
	{
		if (word == NULL)
		{
			break;
		}
		add(word, *test);
		word = strtok(NULL, delimeters);
	}
}
