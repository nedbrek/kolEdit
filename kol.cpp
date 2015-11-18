#include "kol.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Character::SkillDef
{
	uint8_t code[9];
	const char *desc;
};
const Character::SkillDef Character::skillDefs[] =
{
	{{0x81, 0x49, 0xE1, 0x20, 0x70, 0x50, 0x00, 0x00, 0x75}, "Broad Axe"},
	{{0xA1, 0x49, 0xE1, 0x24, 0xEE, 0xF9, 0x10, 0xC0, 0x5D}, "Broadsword"},
	{{0x72, 0xB0, 0xC0, 0x13, 0xEE, 0x00, 0x00, 0x00, 0x0B}, "Elf Bow"},
	{{0xA3, 0xC8, 0xA1, 0xA4, 0xEE, 0xF9, 0x10, 0x03, 0x57}, "Greatsword"},
	{{0x74, 0x05, 0x82, 0x2C, 0x88, 0x00, 0x00, 0x00, 0x7E}, "Halberd"},
	{{0x86, 0x3D, 0xC7, 0x00, 0x9F, 0x70, 0x00, 0x00, 0x0A}, "Longbow"},
	{{0x96, 0x3D, 0xC7, 0x9D, 0xDF, 0x22, 0x68, 0x13, 0x5C}, "Longsword"},
	{{0x46, 0x84, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3C}, "Mace"},
	{{0,}, 0}
};

//----------------------------------------------------------------------------
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

void writeBigEndian(uint8_t *buf, uint32_t sz, uint32_t val)
{
	while (sz)
	{
		--sz;
		buf[sz] = val & 0xff;
		val >>= 8;
	}
}

Character* readChar(FILE *ifile)
{
	Character *cp = new Character;
	cp->read(ifile);
	return cp;
}

//----------------------------------------------------------------------------
bool Character::read(FILE *ifile)
{
	return fread(buf, 1, 768, ifile) == 768;
}

void Character::write(FILE *ofile)
{
	fwrite(buf, 1, 768, ofile);
}

bool Character::valid() const
{
	return buf[0] != 0;
}

const char* Character::name() const
{
	return reinterpret_cast<const char*>(&buf[0]);
}

uint32_t Character::damage      () const { return buf[0x1b]; }
uint32_t Character::classId     () const { return buf[0x1c]; }
uint32_t Character::strength    () const { return scaleStat(buf[0x1d]); }
uint32_t Character::quickness   () const { return scaleStat(buf[0x1e]); }
uint32_t Character::size        () const { return scaleStat(buf[0x1f]); }
uint32_t Character::endurance   () const { return scaleStat(buf[0x20]); }
uint32_t Character::foresight   () const { return scaleStat(buf[0x21]); }
uint32_t Character::charisma    () const { return scaleStat(buf[0x22]); }
uint32_t Character::intelligence() const { return scaleStat(buf[0x23]); }
	//printf("\tFoo  : %d\n", buf[0x24]);
	//printf("\tBar  : %d\n", buf[0x25]);
uint32_t Character::adventurePoints() const { return readBigEndian(&buf[0x26], 2); }
void Character::setAdventurePoints(uint32_t newAp)
{
	writeBigEndian(&buf[0x26], 2, newAp);
}

uint32_t Character::gold() const { return readBigEndian(&buf[0x2a], 3); }

Skill Character::getSkill(uint32_t cnt) const
{
	Skill ret;
	const uint32_t base = 0x4a;
	const uint32_t offset = base + cnt * 13;
	const SkillDef *sd = findSkill(&buf[offset]);
	if (sd)
	{
		ret.desc = sd->desc;
		ret.offense = readBigEndian(&buf[offset+9], 2) / 10;
		ret.defense = readBigEndian(&buf[offset+11], 2) / 10;
	}
	return ret;
}

void Character::setSkill(uint32_t cnt, const Skill &newSkill)
{
	const uint32_t base = 0x4a;
	const uint32_t offset = base + cnt * 13;

	writeBigEndian(&buf[offset+9], 2, newSkill.offense * 10);
	writeBigEndian(&buf[offset+11], 2, newSkill.defense * 10);
}

void Character::print() const
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

	printSkill(0);
	printSkill(1);

	printf("\tQuest");
	printf(" %02x", buf[0x1e2]);
	printf(" %02x", buf[0x1e3]);
	printf(" %02x", buf[0x1e4]);
	printf(" %02x", buf[0x1e5]);
	printf(" %02x", buf[0x1e6]);
	printf(" %02x", buf[0x1e7]);
	printf("\n");
}

uint32_t Character::scaleStat(uint8_t raw)
{
	return raw / 1.5;
}

const Character::SkillDef* Character::findSkill(const uint8_t *buf)
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

void Character::printSkill(uint32_t ct) const
{
	Skill s = getSkill(ct);
	if (!s.desc)
		return;

	printf("\tSkill %d: %s (%d/%d)\n", ct + 1, s.desc, s.offense, s.defense);
}

//----------------------------------------------------------------------------
void Character::loadVec(FILE *ifile, std::vector<Character*> *chars, int *validCtP)
{
	int &validCt = *validCtP;

	Character *c = readChar(ifile);
	validCt = 0;
	while (!feof(ifile))
	{
		chars->push_back(c);
		if (c->valid())
			++validCt;

		c = readChar(ifile);
	}

	delete c;
}

