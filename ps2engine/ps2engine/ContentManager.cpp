#include "ContentManager.h"

void ContentManager::RegisterObject(string guid, AssetObject* obj) {
	objects[guid] = obj;
}

AssetObject* ContentManager::FindObject(string guid) {
	return objects[guid];
}