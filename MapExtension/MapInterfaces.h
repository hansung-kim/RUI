#pragma once
#include <vcl.h>
#include <string>

class TileManager;
class GoogleLayer;
class FlatEarthView;

class IMapProvider {
public:
    virtual void Initialize(bool loadFromInternet, const AnsiString& basePath) = 0;
    virtual TileManager* GetTileManager() = 0;
    virtual GoogleLayer* CreateLayer() = 0;
    virtual ~IMapProvider() {}
};

class IMapView {
public:
    virtual void Resize(int width, int height) = 0;
    virtual void Render() = 0;
    virtual ~IMapView() {}
};
