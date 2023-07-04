#include "beatmap.h"
#include "../logger.h"

Mode ModeFromString(std::string& name) {
  if (name == "Standard")
    return Mode::Standard;
  else if (name == "OneSaber")
    return Mode::OneSaber;
  else if (name == "NoArrows")
    return Mode::NoArrows;
  else if (name == "360Degree")
    return Mode::ThreeSixty;
  else if (name == "90Degree")
    return Mode::Ninety;
  else if (name == "Lightshow")
    return Mode::Lightshow;
  else if (name == "Lawless")
    return Mode::Lawless;
  else
    return Mode::Unknown;
}

static std::string beatmapDirectory;

int LoadDifficulty(Difficulty& difficulty, std::string& filename, Mode mode) {
  std::string path = beatmapDirectory + "/" + filename;
  FILE* file = fopen(path.c_str(), "r");
  if (file != NULL) {
    LOG_ERROR("Failed to open difficulty file: %s\n", path.c_str());
    return -1;
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* data = new char[size];
  fread(data, 1, size, file);
  fclose(file);

  JS::Map map;
  JS::ParseContext parseContext(data, size, map);
  if (parseContext.error != JS::Error::NoError) {
    LOG_ERROR("Failed to parse Json:\n%s\n",
      parseContext.makeErrorString().c_str());
    return -1;
  }

  difficulty = map.castTo<Difficulty>(parseContext);
  if (parseContext.error != JS::Error::NoError) {
    LOG_ERROR("Failed to get Difficulty from %s:\n%s\n", path.c_str(),
      parseContext.makeErrorString().c_str());
    return -1;
  }

  delete[] data;
  return 0;
}

DifficultyList LoadDifficulties(BeatmapInfo& info) {
  DifficultyList difficulties;

  if (info._difficultyBeatmapSets.empty()) {
    LOG_ERROR("No difficulty sets found\n");
    return difficulties;
  }

  auto setIter = info._difficultyBeatmapSets.begin();
  for (; setIter < info._difficultyBeatmapSets.end(); setIter++) {
    DifficultyBeatmapSet& set = *setIter;
    auto mode = ModeFromString(set._beatmapCharacteristicName);
    if (mode == Mode::Unknown) {
      LOG_DEBUG("Unknown mode: %s\n", set._beatmapCharacteristicName.c_str());
      continue;
    }
    
    if (set._difficultyBeatmaps.empty()) {
      LOG_DEBUG("No difficulties found for set %s\n", set._beatmapCharacteristicName.c_str());
      continue;
    }

    auto beatmapIter = set._difficultyBeatmaps.begin();
    for (; beatmapIter != set._difficultyBeatmaps.end(); ++beatmapIter) {
      DifficultyBeatmap& diff = *beatmapIter;
      std::string& filename = diff._beatmapFilename;
      if (filename.empty()) {
        LOG_DEBUG("Empty filename for difficulty\n");
        continue;
      }
      
      Difficulty difficulty;
      if (LoadDifficulty(difficulty, filename, mode) < 0) {
        LOG_ERROR("Failed to load difficulty: %s\n", filename.c_str());
        continue;
      }

      difficulties.push_back(difficulty);
    }
  }

  LOG_DEBUG("Finished retrieving all difficulty sets\n");
  return difficulties;
}

int GetInfoData(BeatmapInfo& info) {
  std::string infoPath = beatmapDirectory + "/Info.dat";
  FILE* file = fopen(infoPath.c_str(), "r");
  if (file == NULL) {
    LOG_ERROR("Failed to open Info.dat: %s\n", infoPath.c_str());
    return -1;
  }

  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char* data = new char[size];
  fread(data, 1, size, file);
  fclose(file);

  JS::Map map;
  JS::ParseContext parseContext(data, size, map);
  if (parseContext.error != JS::Error::NoError) {
    LOG_ERROR("Failed to parse Json:\n%s\n",
      parseContext.makeErrorString().c_str());
    return -1;
  }

  info = map.castTo<BeatmapInfo>(parseContext);
  if (parseContext.error != JS::Error::NoError) {
    LOG_ERROR("Failed to get BeatmapInfo from Info.dat:\n%s\n",
      parseContext.makeErrorString().c_str());
    return -1;
  }

  delete[] data;
  return 0;
}

const char* BeatmapData::getDirectory() {
  return beatmapDirectory.c_str();
}

int BeatmapData::loadMapData(const char *directory) {
  LOG_DEBUG("Clearing old map data\n");
  info.reset();

  if (!difficulties.empty())
    difficulties.clear();

  beatmapDirectory = directory;
  LOG_DEBUG("Getting Map Data from %s\n", directory);

  if (GetInfoData(info) < 0) {
    LOG_ERROR("Failed to get BeatmapInfo from %s\n", directory);
    return -1;
  }

  return 0;
}

int BeatmapData::loadDifficulties() {
  LOG_DEBUG("BeatmapInfo loaded\n");

  DifficultyList difficulties = LoadDifficulties(info);
  if (difficulties.empty()) {
    LOG_ERROR("Failed to load any difficulties from beatmap in %s\n", beatmapDirectory.c_str());
    return -1;
  }

  LOG_DEBUG("Loaded %d difficulties\n", difficulties.size());
  return 0;
}
