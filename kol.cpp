#include <cstdio>
#include <inttypes.h>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct SkillDef
{
	uint8_t code[9];
	const char *desc;
} skillDefs[] =
{
	{{0x81, 0x49, 0xE1, 0x20, 0x70, 0x50, 0x00, 0x00, 0x75}, "Broad Axe"},
	{{0xA1, 0x49, 0xE1, 0x24, 0xEE, 0xF9, 0x10, 0xC0, 0x5D}, "Broadsword"},
	{{0x72, 0xB0, 0xC0, 0x13, 0xEE, 0x00, 0x00, 0x00, 0x0B}, "Elf Bow"},
	{{0xA3, 0xC8, 0xA1, 0xA4, 0xEE, 0xF9, 0x10, 0x03, 0x57}, "Greatsword"},
	{{0x74, 0x05, 0x82, 0x2C, 0x88, 0x00, 0x00, 0x00, 0x7E}, "Halberd"},
	{{0x86, 0x3D, 0xC7, 0x00, 0x9F, 0x70, 0x00, 0x00, 0x0A}, "Longbow"},
	{{0x96, 0x3D, 0xC7, 0x9D, 0xDF, 0x22, 0x68, 0x13, 0x5C}, "Longsword"},
};

uint32_t readBigEndian(const uint8_t *buf, uint32_t sz)
{
	uint32_t ret = 0;

	while (sz)
	{
		ret <<= 8;
		ret += *buf;

		++buf;
		--sz;
	}

	return ret;
}

SkillDef* findSkill(const uint8_t *buf)
{
	int i = 0;
	while (skillDefs[i].desc)
	{
		if (memcmp(skillDefs[i].code, buf, 9) == 0)
			return &skillDefs[i];

		++i;
	}
	return 0;
}

void printSkill(const uint8_t *buf, uint32_t ct)
{
	const uint32_t base = 0x4a;
	const uint32_t offset = base + ct * 13;
	SkillDef *sd = findSkill(&buf[offset]);
	if (sd)
	{
		const uint32_t off = readBigEndian(&buf[offset+9], 2) / 10;
		const uint32_t def = readBigEndian(&buf[offset+11], 2) / 10;
		printf("\tSkill %d: %s (%d/%d)\n", ct + 1, sd->desc, off, def);
	}
}

class Character
{
public:
	Character()
	{
	}

	void read(FILE *ifile)
	{
		fread(buf, 1, 768, ifile);
	}

	void write(FILE *ofile)
	{
		fwrite(buf, 1, 768, ofile);
	}

	bool valid() const { return buf[0] != 0; }

	const char* name() const { return reinterpret_cast<const char*>(&buf[0]); }
	uint32_t damage() const { return buf[0x1b]; }
	uint32_t classId() const { return buf[0x1c]; }
	uint32_t strength() const { return scaleStat(buf[0x1d]); }
	uint32_t quickness() const { return scaleStat(buf[0x1e]); }
	uint32_t size() const { return scaleStat(buf[0x1f]); }
	uint32_t endurance() const { return scaleStat(buf[0x20]); }
	uint32_t foresight() const { return scaleStat(buf[0x21]); }
	uint32_t charisma() const { return scaleStat(buf[0x22]); }
	uint32_t intelligence() const { return scaleStat(buf[0x23]); }
	//printf("\tFoo  : %d\n", buf[0x24]);
	//printf("\tBar  : %d\n", buf[0x25]);
	uint32_t adventurePoints() const { return readBigEndian(&buf[0x26], 2); }
	uint32_t gold() const { return readBigEndian(&buf[0x2a], 3); }

	void print() const
	{
		if (!valid())
			return;

		printf("Name: %s\n", name());

		printf("\tDam  : %d\n", damage());
		printf("\tClass: %d\n", classId());
		printf("\tStr  : %d\n", strength());
		printf("\tQuick: %d\n", quickness());
		printf("\tSize : %d\n", size());
		printf("\tEnd  : %d\n", endurance());
		printf("\tFore : %d\n", foresight());
		printf("\tChsma: %d\n", charisma());
		printf("\tIntel: %d\n", intelligence());

		printf("\tAP: %d\n", adventurePoints());

		printf("\tBar  : %d\n", buf[0x28]);
		printf("\tFoo  : %d\n", buf[0x29]);

		printf("\tGold: %d\n", gold());

		printSkill(&buf[0], 0);
		printSkill(&buf[0], 1);

		printf("\tQuest");
		printf(" %02x", buf[0x1e2]);
		printf(" %02x", buf[0x1e3]);
		printf(" %02x", buf[0x1e4]);
		printf(" %02x", buf[0x1e5]);
		printf(" %02x", buf[0x1e6]);
		printf(" %02x", buf[0x1e7]);
		printf("\n");
	}

private: // methods
	static
	uint32_t scaleStat(uint8_t raw)
	{
		return raw / 1.5;
	}

private: // data
	uint8_t buf[768];
};

Character* readChar(FILE *ifile)
{
	Character *cp = new Character;
	cp->read(ifile);
	return cp;
}

int main(int argc, char **argv)
{
	FILE *ifile = fopen(argv[1], "rb");
	FILE *ofile = fopen(argv[2], "wb");

	Character *c = readChar(ifile);
	std::vector<Character*> chars;
	int validCt = 0;
	while (!feof(ifile))
	{
		chars.push_back(c);
		if (c->valid())
			++validCt;

		c = readChar(ifile);
	}
	delete c;

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

