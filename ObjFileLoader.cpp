// turn off fopen warnings
#define _CRT_SECURE_NO_WARNINGS

#include "ObjFileLoader.h"

ObjFileLoader::ObjFileLoader(char* filename, Obj& objOut)
{
	if (!LoadFile(filename))
		return;

	ParseFile(filename);

	Obj* loadedObj = new Obj();

	loadedObj->texCoordList = texcoord_list;
	loadedObj->normalList = normal_list;
	loadedObj->positionList = position_list;

	loadedObj->positionIndex = pIndex;
	loadedObj->texCoordIndex = tIndex;
	loadedObj->normalIndex = nIndex;

	objOut = *loadedObj;

	delete loadedObj;

	delete[] fbuffer; // delete file buffer created in LoadFile()
}


// load wavefront object file. adds terminating \n so last line of file can be correctly parsed as a 'line' later
// basic loader - only deals with vertices v, texcoords vt, normals vn 
//              - only copes with triangular meshes (no quads)
//              - doesn't deal with textures or materials
bool ObjFileLoader::LoadFile(char* fname)
{
	FILE* pFile;

	pFile = fopen(fname , "r"); // if changed to bin format will read carriage return \r (0d) as well as \n (0a) into fbuffer, may need to add \r checks(but seemed to work with basic test)
	if (pFile==NULL) 
	{
		OutputDebugString(L"Failed to open model file");
		OutputDebugStringA(fname);
		return false ;
	}

	// Get file size
	fseek(pFile, 0, SEEK_END);
	fbuffersize = ftell(pFile);
	rewind(pFile);

	// Allocate memory for entire file size
	fbuffer  = new char[fbuffersize+1]; // 1 added to cope with adding a \n later in case file doesn't end with \n 
	if (fbuffer == NULL)
	{
		fclose(pFile);
		OutputDebugString(L"Failed allocate memory for model file");
		OutputDebugStringA(fname);
		return false;
	}

	// Copy file into memory
	actualsize = fread(fbuffer,1,fbuffersize,pFile); // actualsize may be less than fbuffersize in text mode as \r are stripped
	if (actualsize == 0)
	{
		fclose(pFile);
		OutputDebugString(L"Failed to read model file");
		OutputDebugStringA(fname);
		return false;
	}

	// Add a newline at end in case file does not, so can deal with whole buffer as a set of lines of text
	fbuffer[actualsize] = '\n'; fclose(pFile);

	return true;
}
 

// uses concept of getting parsable tokens seperated by whitespace and '/'
// one line of file is parsed at a time, lines seperated by '\n'
void ObjFileLoader::ParseFile(string filename)
{
	tokenptr=0; // token pointer points to first element of buffer

	int tokenstart, tokenlength;

	xyz tempxyz;
	xy tempxy;

	bool success;
	int line=0;

	do
	{	
		line++; // keep track of current line number for error reporting

		if(!getnexttoken(tokenstart, tokenlength)) continue; // get first token on line, go to next line if first token is \n

		// ADD FURTHER KEYWORDS HERE TO EXTEND CAPABILITIES
		if(strncmp(&fbuffer[tokenstart], "v ", 2)==0) // VERTEX POSITION - note the space in the string is needed (see vt, etc)
		{
			success=true; // used to see if correct number of tokens left on line for this type of attribute
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.x = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.y = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.z = (float) atof(&fbuffer[tokenstart]);

			// if not correct number of tokens, display error in debug output
			if(!success) {char s[100] = "ERROR: Badly formatted vertex, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  OutputDebugStringA(s); }

			position_list.push_back(tempxyz); // add a new element to the list

		}
		else if(strncmp(&fbuffer[tokenstart], "vt", 2)==0) // TEXTURE COORDINATES
		{
			success=true;
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxy.x = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxy.y = (float) atof(&fbuffer[tokenstart]);

			if(!success) {char s[100] = "ERROR: Badly formatted texture coordinate, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  OutputDebugStringA(s); }

			texcoord_list.push_back(tempxy);
		}
		else if(strncmp(&fbuffer[tokenstart], "vn", 2)==0)  // NORMALS
		{
			success=true;
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.x = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.y = (float) atof(&fbuffer[tokenstart]);
			success = success && getnexttoken(tokenstart, tokenlength);
			tempxyz.z = (float) atof(&fbuffer[tokenstart]);

			if(!success) {char s[100] = "ERROR: Badly formatted normal, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  OutputDebugStringA(s); }

			normal_list.push_back(tempxyz);
		}
		else if(strncmp(&fbuffer[tokenstart], "f ", 2)==0)  // FACE - only deals with triangles so far
		{
			int tempptr = tokenstart + 2; // skip "f "
			int forwardslashcount=0;
			bool adjacentslash = false;

			// this works out how many elements are specified for a face, e.g.
			// f 1 2 3				-> 0 forward slashes = just position
			// f 1/1 2/2 3/3		-> 3 slashes = position and texcoords
			// f 1/1/1 2/2/2 3/3/3	-> 6 slashes = position, texcoords, normals
			// f 1//1 2//2 3//3		-> 6 slashes with adjacent = position, normals
			while(fbuffer[tempptr] != '\n')
			{
				if(fbuffer[tempptr] == '/')
				{
					forwardslashcount++;
					if(fbuffer[tempptr-1] == '/') adjacentslash=true;
				}
				tempptr++;
			}

			success=true;

			// Get 3 sets of indices per face
			for(int i=0; i<3; i++)
			{
				// get vertex index
				success = success && getnexttoken(tokenstart, tokenlength);
				pIndex.push_back(atoi(&fbuffer[tokenstart]));

				if(forwardslashcount>=3&& adjacentslash==false) // get texcoord index if required 
				{
					success = success && getnexttoken(tokenstart, tokenlength);
					tIndex.push_back(atoi(&fbuffer[tokenstart]));
				}

				if(forwardslashcount==6 || adjacentslash==true) // get normal index if required 
				{
					success = success && getnexttoken(tokenstart, tokenlength);
					nIndex.push_back(atoi(&fbuffer[tokenstart]));
				}
			}

			if(!success) {char s[100] = "ERROR: Badly formatted face, line : "; _itoa(line, &s[strlen(s)], 10); strcat(s, " : "); strcat(s, filename.c_str());  OutputDebugStringA(s); }
		}
	} while(getnextline() == true);
}


// get next token. if \n is next token do not proceed, use getnextline() to resume
bool ObjFileLoader::getnexttoken(int& tokenstart, int& tokenlength)
{
	tokenstart = tokenptr; 
	tokenlength=1; 
	int tokenend;

	while(fbuffer[tokenptr] == ' ' || fbuffer[tokenptr] == '\t' || fbuffer[tokenptr] == '/') tokenptr++; //skip whitespace and '/'

	if(fbuffer[tokenptr] == '\n') { return false; } // keeps tokenptr pointing to \n as a token to indicate end of line
													// doesn't point to next token, dealt with in getnextline()
	tokenend=tokenptr+1;

	while(fbuffer[tokenend] != ' ' && fbuffer[tokenend] != '\t' && fbuffer[tokenend] != '\n' && fbuffer[tokenend] != '/') tokenend++; // find length of token by finding next whitespace or '\n' or '/'

	tokenlength = tokenend - tokenptr;
	tokenstart = tokenptr;
	tokenptr+=tokenlength; //ready for next token

	return true;
}


// gets next line of buffer by skipping to next element after end of current line, returns false when end of buffer exceeded
bool ObjFileLoader::getnextline()
{
	// relies on getnexttoken()leaving tokenptr pointing to \n if encountered

	while(fbuffer[tokenptr] != '\n' && tokenptr < actualsize) tokenptr++; // skip to end of line

	tokenptr++; // point to start of next line

	if (tokenptr >= actualsize) return false;
	else return true;
}




ObjFileLoader::~ObjFileLoader()
{
	delete [] vertices;

	position_list.clear();
	normal_list.clear();
	texcoord_list.clear();

	//loadedModels.clear();
}
