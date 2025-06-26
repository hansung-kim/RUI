#include "FlatEarthMapView.h"
#include "FlatEarthView.h"
#include "GoogleLayer.h"

FlatEarthMapView::FlatEarthMapView(GoogleLayer *layer) {
    view = new FlatEarthView(layer);
}

void FlatEarthMapView::Resize(int width, int height) {
    view->Resize(width, height);
}

void FlatEarthMapView::Render() {
    view->Render(true);
}

FlatEarthView* FlatEarthMapView::GetView() {
    return view;
}
