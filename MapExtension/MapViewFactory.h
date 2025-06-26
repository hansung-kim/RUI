#pragma once
#include "MapInterfaces.h"

class MapViewFactory {
public:
    static IMapView* Create(int mapType, bool loadFromInternet, const AnsiString& exePath);
};
