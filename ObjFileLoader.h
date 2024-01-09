#pragma once

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT

#include <d3d11.h>
#include <DirectXMath.h>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;


struct xyz { float x, y, z; };	//used for vertices and normals during file parse
struct xy { float x, y; };		//used for texture coordinates during file parse

struct Obj
{
	std::vector <xyz> positionList;		
	std::vector <xyz> normalList;
	std::vector <xy> texCoordList;		

	std::vector <int> positionIndex;
	std::vector <int> texCoordIndex;
	std::vector <int> normalIndex;
};

class ObjFileLoader
{
private:

	bool LoadFile(char* fname);

	char* fbuffer;
	long fbuffersize; // filesize
	size_t actualsize; // actual size of loaded data (can be less if loading as text files as ASCII CR (0d) are stripped out)


	void ParseFile(string filename);
	bool getnextline() ;
	bool getnexttoken(int& tokenstart, int& tokenlength);

	unsigned int  tokenptr;


public:
	ObjFileLoader(char* filename, Obj& objOut);
	~ObjFileLoader();

	// Define model vertex structure
	struct MODEL_POS_COL_TEX_NORM_VERTEX
	{
		DirectX::XMFLOAT3 Pos;
		DirectX::XMFLOAT4 Col;
		DirectX::XMFLOAT2 TexCoord;
		DirectX::XMFLOAT3 Normal;
	};

	//string filename;

	vector <xyz> position_list;		// list of parsed positions
	vector <xyz> normal_list;		// list of parsed normals
	vector <xy> texcoord_list;		// list of parsed texture coordinates

	// Lists of indices into above lists derived from faces
	vector <int> pIndex; 
	vector <int> tIndex;
	vector <int> nIndex;

	MODEL_POS_COL_TEX_NORM_VERTEX* vertices;
	unsigned int numverts;


};


