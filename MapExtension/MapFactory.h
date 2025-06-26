#pragma once
#include <map>
#include <functional>

class IMapProvider;

class MapFactory {
public:
    using CreatorFunc = std::function<IMapProvider*()>;

    static MapFactory& Instance();
    void Register(int type, CreatorFunc func);
    IMapProvider* Create(int type);

private:
    std::map<int, CreatorFunc> creators;
    MapFactory() {}
};
