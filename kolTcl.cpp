#include "kol.h"
#include "tclparms.h"

struct KolData
{
public:
	KolData()
	{
	}

	bool load(const char *filename)
	{
		FILE *ifile = fopen(filename, "rb");
		if (!ifile)
			return false;

		int validCt = 0;
		Character::loadVec(ifile, &chars_, &validCt);
		return validCt > 0;
	}

	void save(const char *filename)
	{
		FILE *ofile = fopen(filename, "wb");
		if (!ofile)
			return;

		for (std::vector<Character*>::const_iterator i = chars_.begin(); i != chars_.end(); ++i)
		{
			(**i).write(ofile);
		}
	}

private:
	std::vector<Character*> chars_;
};

static
int loadCmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	if (objc < 2)
	{
		Tcl_AppendResult(interp, "Usage: kol::load <filename>", NULL);
		return TCL_ERROR;
	}

	Parms p(interp, objv, unsigned(objc));

	return static_cast<KolData*>(cdata)->load(p.getStringParm(0)) ? TCL_OK : TCL_ERROR;
}

static
int saveCmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	if (objc < 2)
	{
		Tcl_AppendResult(interp, "Usage: kol::save <filename>", NULL);
		return TCL_ERROR;
	}

	Parms p(interp, objv, unsigned(objc));
	static_cast<KolData*>(cdata)->save(p.getStringParm(0));
	return TCL_OK;
}

static
int charCmd(ClientData cdata, Tcl_Interp *interp, int objc, Tcl_Obj *const objv[])
{
	return TCL_OK;
}

extern "C"
int Koledit_Init(Tcl_Interp *interp)
{
	KolData *data = new KolData;

	Tcl_CreateObjCommand(interp, "kol::load", loadCmd, data, NULL);
	Tcl_CreateObjCommand(interp, "kol::save", saveCmd, data, NULL);
	Tcl_CreateObjCommand(interp, "kol::char", charCmd, data, NULL);

	return TCL_OK;
}

