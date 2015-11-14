#include "kol.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	FILE *ifile = fopen(argv[1], "rb");
	FILE *ofile = fopen(argv[2], "wb");

	int validCt = 0;
	std::vector<Character*> chars;
	Character::loadVec(ifile, &chars, &validCt);

	printf("Read %ld characters, %d valid\n", chars.size(), validCt);

	std::string cmd;
	std::getline(std::cin, cmd);
	while (std::cin)
	{
		if (cmd[0] == 'q')
			break;

		std::istringstream is(cmd);
		int idx = 0;
		is >> idx;
		if (chars[idx]->valid())
			chars[idx]->print();
		else
			printf("Character not valid\n");

		std::getline(std::cin, cmd);
	}

	for (std::vector<Character*>::const_iterator i = chars.begin(); i != chars.end(); ++i)
	{
		(**i).write(ofile);
	}

	fclose(ifile);
	fclose(ofile);
	return 0;
}

