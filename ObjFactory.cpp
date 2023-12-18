#include "ObjFactory.h"


Obj* ObjFactory::Cube()
{
	vector <xyz> tempPosList;
	vector <xyz> tempNormList;
	vector <xy> tempTexList;

	ObjFileModel* objToLoad = new ObjFileModel(const_cast<char*>("Models/cube.obj"), tempPosList, tempNormList, tempTexList);

	Obj* obj = new Obj();

	obj->position_list = tempPosList;
	obj->normal_list = tempNormList;
	obj->texcoord_list = tempTexList;

	delete objToLoad;

	return obj;
}
