#pragma once
#include "MapInterfaces.h"

class FlatEarthMapView : public IMapView {
private:
    FlatEarthView *view;

public:
    FlatEarthMapView(GoogleLayer *layer);
    void Resize(int width, int height) override;
    void Render() override;
    FlatEarthView* GetView();
};
