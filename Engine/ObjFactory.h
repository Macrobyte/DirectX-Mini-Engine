#pragma once
#include <map>

#include "./Utility/ObjFileLoader.h"

//TODO - Figure out how to make this work. Do I make ObjFileLoader just a file loader?
class ObjFactory
{
public:
	static Obj* GetModel(std::string id) { return models[id]; }
	
	static Obj* Cube();

private:
	ObjFileLoader* objToLoad;

	static map<std::string, Obj*> models;


};

