#include "beatmap.h"


LoadedMapData::LoadedMapData(BeatmapInfo info, std::vector<Difficulty> difficulties) {
  this->info = info;
  this->difficulties = difficulties;
}

LoadedMapData::~LoadedMapData() {

}


LoadedMap::LoadedMap(LoadedMapData& mapData, char* coverImageData, Track* song) : map_data(mapData) {
  this->song = song;
}

LoadedMap::~LoadedMap() {

}