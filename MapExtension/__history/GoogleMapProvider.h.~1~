#pragma once
#include "MapInterfaces.h"

class FilesystemStorage;
class KeyholeConnection;

class GoogleMapProvider : public IMapProvider {
private:
    FilesystemStorage* storage;
    KeyholeConnection* keyhole;
    TileManager* tileManager;

public:
    void Initialize(bool loadFromInternet, const AnsiString& basePath) override;
    TileManager* GetTileManager() override;
    GoogleLayer* CreateLayer() override;
};
