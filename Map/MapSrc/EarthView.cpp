//---------------------------------------------------------------------------


#pragma hdrstop

#include "EarthView.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
EarthView::EarthView(MasterLayer *ml) {
	m_MasterLayer = ml;
	m_ViewportWidth = m_ViewportHeight = 0;
}

EarthView::~EarthView() {
}

void EarthView::Resize(int width, int height) {
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}

/* mouse movements */
int EarthView::StartDrag(int x, int y, int flags) {
	return 0;
}

int EarthView::Drag(int fromx, int fromy, int x, int y, int flags) {
	return 0;
}

int EarthView::Click(int x, int y, int flags) {
	return 0;
}

/* keyboard movements */
int EarthView::StartMovement(int flags) {
	return 0;
}

int EarthView::StopMovement(int flags) {
	return 0;
}

int EarthView::SingleMovement(int flags) {
	return 0;
}
#define MIN_HEIGHT 10.0/40000000.0
#define MAX_HEIGHT 1.0
double EarthView::GetCurrentZoom() {
    return MAX_HEIGHT / m_Eye.h;
}
