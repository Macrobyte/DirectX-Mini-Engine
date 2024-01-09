#pragma once
#include <map>

#include "ObjFileLoader.h"

class ObjLibrary
{
	static map<std::string, Obj*> models;
	static Obj* Cube();
	static Obj* Plane();
	static Obj* Sphere();
	static Obj* Scream();
	static Obj* GravityGun();
	static Obj* Gun();

public:

	static Obj* GetModel(std::string id) { return models[id]; }
	
	static map<std::string, Obj*> GetModels() { return models; }

	static void LoadAllModels();
};

