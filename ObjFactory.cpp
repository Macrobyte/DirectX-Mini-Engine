#include "ObjFactory.h"


Obj* ObjFactory::Cube()
{
	vector <xyz> tempPosList;
	vector <xyz> tempNormList;
	vector <xy> tempTexList;

	Obj* obj = new Obj();

	ObjFileLoader* objToLoad = new ObjFileLoader(const_cast<char*>("Models/cube.obj"), *obj);

	delete objToLoad;

	return obj;
}
