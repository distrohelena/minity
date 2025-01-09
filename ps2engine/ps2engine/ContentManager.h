#pragma once
#include <map>
#include <string>
#include "AssetObject.h"
using namespace std;

class ContentManager
{
private:
	map<string, AssetObject*> objects;

public:
	void RegisterObject(string guid, AssetObject* obj);
	AssetObject* FindObject(string guid);

};

