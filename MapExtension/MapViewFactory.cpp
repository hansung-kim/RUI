#include "MapViewFactory.h"
#include "MapFactory.h"
#include "FlatEarthMapView.h"
#include "GoogleLayer.h"
#include "FlatEarthView.h"
#include <System.SysUtils.hpp>

IMapView* MapViewFactory::Create(int mapType, bool loadFromInternet, const AnsiString& exePath) {
    IMapProvider* provider = MapFactory::Instance().Create(mapType);
    if (!provider)
        throw Sysutils::Exception("Invalid map type");

    provider->Initialize(loadFromInternet, exePath);
    GoogleLayer* layer = provider->CreateLayer();

    return new FlatEarthMapView(layer);
}
