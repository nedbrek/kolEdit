#ifndef KOL_CHAR_H
#define KOL_CHAR_H

#include <cstdio>
#include <inttypes.h>
#include <vector>

/// One skill
struct Skill
{
	const char *desc;
	uint32_t offense;
	uint32_t defense;

	Skill()
	: desc(0)
	, offense(0)
	, defense(0)
	{
	}
};

/// Hold the character data from disk
class Character
{
public:
	Character()
	{
	}

	bool read(FILE *ifile);
	void write(FILE *ofile);

	bool valid() const;

	const char* name() const;
	uint32_t damage() const;
	uint32_t classId() const;
	uint32_t strength() const;
	uint32_t quickness() const;
	uint32_t size() const;
	uint32_t endurance() const;
	uint32_t foresight() const;
	uint32_t charisma() const;
	uint32_t intelligence() const;
	uint32_t adventurePoints() const;
	uint32_t gold() const;

	Skill getSkill(uint32_t cnt) const;

	void print() const;

	void setAdventurePoints(uint32_t newAp);

	void setSkill(uint32_t cnt, const Skill &newSkill);

	static
	void loadVec(FILE *ifile, std::vector<Character*> *chars, int *validCtP);

private: // methods
	struct SkillDef;
	static const SkillDef skillDefs[];

	static
	uint32_t scaleStat(uint8_t raw);

	static
	const SkillDef* findSkill(const uint8_t *buf);

	void printSkill(uint32_t ct) const;

private: // data
	uint8_t buf[768];
};

#endif

