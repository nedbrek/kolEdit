#ifndef KOL_CHAR_H
#define KOL_CHAR_H

#include <cstdio>
#include <inttypes.h>

class Character
{
public:
	Character()
	{
	}

	void read(FILE *ifile);
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

	void print() const;

private: // methods
	static
	uint32_t scaleStat(uint8_t raw);

private: // data
	uint8_t buf[768];
};

#endif

