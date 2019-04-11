#pragma once

namespace StringVector
{
	struct data
	{
		char **str;
		int array_size;
		int n_count;

	};

	void init(data *d)
	{
		d->array_size = 10;
		d->n_count = 0;
		d->str = new char*[d->array_size];

	}

	void resize(data *d)
	{
		d->array_size *= 1.2;
		d->array_size++;
		char **tmp = new char *[d->array_size];
		for (int i = 0; i < d->n_count; i++)
		{
			tmp[i] = d->str[i];
		}
		delete[] d->str;
		d->str = tmp;
	}

	void add(data *d, char *s)
	{
		if (d->n_count >= d->array_size) resize(d);
		d->str[d->n_count] = s;
		d->n_count++;
	}

}

namespace TrackNames
{
	struct data
	{
		char **name;
		int array_size;
		int n_count;
	};

	void init(data *d)
	{
		d->array_size = 50;
		d->n_count = 0;
		d->name = new char*[d->array_size];
	}

	void resize(data *d)
	{
		d->array_size *= 1.2;
		d->array_size++;
		char **tmp = new char*[d->array_size];
		for (int i = 0; d->n_count; i++)
		{
			tmp[i] = d->name[i];
		}
		delete[] d->name;
		d->name = tmp;
	}

	void add(data *d, char *s)
	{
		if (d->n_count >= d->array_size) resize(d);
		d->name[d->n_count] = s;
		d->n_count++;
	}
}

namespace twitch_messages
{
	struct data
	{
		char **msg;
		int array_size;
		int n_count;
		int *duplicate;
	};

	void init(data *d)
	{
		d->array_size = 100;
		d->n_count = 0;
		d->msg = new char*[d->array_size];
		d->duplicate[d->array_size] = 0;
	}

	void resize(data *d)
	{
		d->array_size *= 1.2;
		d->array_size++;
		char **tmp = new char*[d->array_size];
		for (int i = 0; i < d->n_count; i++)
		{
			tmp[i] = d->msg[i];
		}
		delete[] d->msg;
		d->msg = tmp;
	}

	void add(data *d, char *s)
	{
		if (d->n_count >= d->array_size) resize(d);
		d->msg[d->n_count] = s;
		d->n_count++;
	}

}

char *sort(twitch_messages::data *d)
{
	for (int i = 0; i < d->n_count - 1; i++)
	{
		for (int j = i + 1; j < d->n_count; j++)
		{
			if (strcmp(d->msg[i], d->msg[j]) < 0)
			{
				swap(d->msg[i], d->msg[j]);

				if (strcmp(d->msg[i], d->msg[j]) == 0)
				{
					d->duplicate[i]++;
					cout << d->duplicate[i] << endl;
				}

			}
		}
	}

	for (int i = 0; i < d->n_count; i++)
	{
		for (int j = 0; j < d->n_count; j++)
		{
			if (d->duplicate[i] > d->duplicate[j])
			{

			}
		}
	}


	return *d->msg;
}

