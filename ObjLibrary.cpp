#include "ObjLibrary.h"


map<std::string, Obj*> ObjLibrary::models;

void ObjLibrary::LoadAllModels()
{
	Cube();
	Sphere();
	Scream();
	Plane();
	GravityGun();
	Gun();
}

Obj* ObjLibrary::Cube()
{
	Obj* obj = new Obj();

	ObjFileLoader* objToLoad = new ObjFileLoader(const_cast<char*>("Models/cube.obj"), *obj);

	models["cube"] = obj;

	delete objToLoad;

	return obj;
}

Obj* ObjLibrary::Plane()
{	
	Obj* obj = new Obj();

	ObjFileLoader* objToLoad = new ObjFileLoader(const_cast<char*>("Models/Floor.obj"), *obj);

	models["plane"] = obj;

	delete objToLoad;

	return obj;
}

Obj* ObjLibrary::Sphere()
{
	Obj* obj = new Obj();

	ObjFileLoader* objToLoad = new ObjFileLoader(const_cast<char*>("Models/Sphere.obj"), *obj);

	models["sphere"] = obj;

	delete objToLoad;

	return obj;
}

Obj* ObjLibrary::Scream()
{
	Obj* obj = new Obj();

	ObjFileLoader* objToLoad = new ObjFileLoader(const_cast<char*>("Models/Scream.obj"), *obj);

	models["scream"] = obj;

	delete objToLoad;

	return obj;
}

Obj* ObjLibrary::GravityGun()
{
	Obj* obj = new Obj();

	ObjFileLoader* objToLoad = new ObjFileLoader(const_cast<char*>("Models/gravityGun.obj"), *obj);

	models["gravityGun"] = obj;

	delete objToLoad;

	return obj;
}

Obj* ObjLibrary::Gun()
{
	Obj* obj = new Obj();

	ObjFileLoader* objToLoad = new ObjFileLoader(const_cast<char*>("Models/gun.obj"), *obj);

	models["gun"] = obj;

	delete objToLoad;

	return obj;
	
}
