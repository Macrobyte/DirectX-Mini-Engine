#pragma once
#include <map>

#include "ObjFileModel.h"

//TODO - Figure out how to make this work. Do I make ObjFileModel just a file loader?
class ObjFactory
{
public:
	static Obj* GetModel(std::string id) { return models[id]; }
	
	static Obj* Cube();

private:
	ObjFileModel* objToLoad;

	static map<std::string, Obj*> models;


};

