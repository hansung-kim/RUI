#include "MapFactory.h"

MapFactory& MapFactory::Instance() {
    static MapFactory instance;
    return instance;
}

void MapFactory::Register(int type, CreatorFunc func) {
    creators[type] = func;
}

IMapProvider* MapFactory::Create(int type) {
    auto it = creators.find(type);
    if (it != creators.end())
        return it->second();
    return nullptr;
}
