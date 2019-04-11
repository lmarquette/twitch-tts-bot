#pragma once
#include <stdio.h>
#include <stdlib.h>

const long long int buffer_size = 10000;


int ReadingLines(char **lines, const char *filename)
{
	FILE *fp = fopen(filename, "r");
	int line_count = 0;
	static char buffer[buffer_size];
	lines = (char**)malloc(sizeof(char*) * buffer_size);

	while (fgets(buffer, sizeof(buffer), fp) != NULL)//Run until it reaches the end of the text file
	{
		lines[line_count] = (char*)malloc(sizeof(char) * buffer_size);
		//printf("%s", buffer);
		strcpy(lines[line_count], buffer); //storing from buffer to lines pointer
		printf("%s", lines[line_count]);
		line_count++;
		//printf("%s", lines[line_count]);
	}

	return line_count;
}