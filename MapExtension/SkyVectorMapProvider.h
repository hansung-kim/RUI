#pragma once
#include "MapInterfaces.h"

class FilesystemStorage;
class KeyholeConnection;

class SkyVectorMapProvider : public IMapProvider {
private:
    int mapType;
    AnsiString mapSubDir;
    FilesystemStorage* storage;
    KeyholeConnection* keyhole;
    TileManager* tileManager;

public:
    SkyVectorMapProvider(int type, const AnsiString& subDir);
    void Initialize(bool loadFromInternet, const AnsiString& basePath) override;
    TileManager* GetTileManager() override;
    GoogleLayer* CreateLayer() override;
};
