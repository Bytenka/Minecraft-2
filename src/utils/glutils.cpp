#include "pch.h"

#include "glutils.h"
#include "Log.h"

#include "Exceptions.h"

namespace tk
{
std::string read_file(const char *filename)
{
	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL)
	{
		throw RuntimeException("Unable to open file \"" + std::string(filename) + "\"!");
	}

	std::string data;
	char c;
	for (unsigned i = 0; (c = fgetc(fp)) != EOF; i++)
	{
		data.push_back(c);
	}

	return data;
}
}; // namespace tk