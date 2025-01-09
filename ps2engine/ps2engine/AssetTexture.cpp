#include "AssetTexture.h"

AssetTexture::~AssetTexture() {
#ifdef DIRECTX
  if (texture) texture->Release();
  if (srv) srv->Release();
#endif
}