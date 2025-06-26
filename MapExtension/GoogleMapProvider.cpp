#include "GoogleMapProvider.h"
#include "TileManager.h"
#include "GoogleLayer.h"
#include "KeyholeConnection.h"
#include "FilesystemStorage.h"
#include <System.SysUtils.hpp>
#include <io.h>
#include <direct.h>
#include <errno.h>

void GoogleMapProvider::Initialize(bool loadFromInternet, const AnsiString& basePath) {
    AnsiString path = basePath + "..\\GoogleMap" + (loadFromInternet ? "_Live\\" : "\\");
    std::string cachedir = path.c_str();

    if (mkdir(cachedir.c_str()) != 0 && errno != EEXIST)
        throw Sysutils::Exception("Can not create cache directory");

    storage = new FilesystemStorage(cachedir, true);
    if (loadFromInternet) {
        keyhole = new KeyholeConnection(GoogleMaps);
        keyhole->SetSaveStorage(storage);
        storage->SetNextLoadStorage(keyhole);
    }
    tileManager = new TileManager(storage, true);
}

TileManager* GoogleMapProvider::GetTileManager() {
    return tileManager;
}

GoogleLayer* GoogleMapProvider::CreateLayer() {
    return new GoogleLayer(tileManager);
}
