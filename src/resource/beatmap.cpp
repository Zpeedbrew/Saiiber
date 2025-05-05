#include "beatmap.h"

#include "../logger.h"

Mode ModeFromString(const char* name) {
  if (strcmp(name, "Standard") == 0)
    return Mode::Standard;
  else if (strcmp(name, "OneSaber") == 0)
    return Mode::OneSaber;
  /*
  else if (strcmp(name, "NoArrows") == 0)
    return Mode::NoArrows;
  else if (strcmp(name, "360Degree") == 0)
    return Mode::ThreeSixty;
  else if (strcmp(name, "90Degree") == 0)
    return Mode::Ninety;
  else if (strcmp(name, "Lightshow") == 0)
    return Mode::Lightshow;
  else if (strcmp(name, "Lawless") == 0)
    return Mode::Lawless;
  else
    return Mode::Unknown;
*/
  return Mode::OneSaber;
}

Rank RankFromString(const char* name) {
  if (strcmp(name, "Easy") == 0)
    return Rank::Easy;
  else if (strcmp(name, "Normal") == 0)
    return Rank::Normal;
  else if (strcmp(name, "Hard") == 0)
    return Rank::Hard;
  else if (strcmp(name, "Expert") == 0)
    return Rank::Expert;
  else if (strcmp(name, "imposible") == 0)
    return Rank::ExpertPlus;
  else
    return Rank::Normal;
}

static std::string beatmapDirectory;

int LoadDifficulty(std::string& path, Difficulty& difficulty) {
  FILE* file = fopen(path.c_str(), "r");
  if (file == NULL) {
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

Beatmap::Beatmap(std::string directory, BeatmapInfo info)
    : directory(directory), info(info) {}

int Beatmap::loadMap(Mode mode, Rank rank) {
  DifficultyBeatmapSet* set = getDifficultySet(mode);
  if (set == NULL) {
    LOG_ERROR("No difficulty set found for mode %d\n", mode);
    return -1;
  }

  DifficultyBeatmap* beatmap = NULL;
  for (auto& diff : set->_difficultyBeatmaps) {
    if (RankFromString(diff._difficulty.c_str()) == rank) {
      beatmap = &diff;
      break;
    }
  }

  if (beatmap == NULL) {
    LOG_ERROR("No difficulty found for mode %d and rank %d\n", mode, rank);
    return -1;
  }

  std::string path = directory + "/" + beatmap->_beatmapFilename;
  if (LoadDifficulty(path, map) != 0) {
    LOG_ERROR("Failed to load difficulty: %s\n", path.c_str());
    return -1;
  }

  LOG_DEBUG("Loaded difficulty: %s\n", path.c_str());
  return 0;
}

DifficultyBeatmapSet* Beatmap::getDifficultySet(Mode mode) {
  for (auto& set : info._difficultyBeatmapSets) {
    if (ModeFromString(set._beatmapCharacteristicName.c_str()) == mode) {
      return &set;
    }
  }

  return NULL;
}

int BeatmapInfo::getModes() {
  int modes = 0;
  for (auto& set : _difficultyBeatmapSets) {
    Mode mode = ModeFromString(set._beatmapCharacteristicName.c_str());
    if (mode != Mode::Unknown) modes |= (1 << (int)mode);
  }

  return modes;
}

int BeatmapInfo::getDifficulties(Mode mode) {
  int difficulties = 0;
  for (auto& set : _difficultyBeatmapSets) {
    if (ModeFromString(set._beatmapCharacteristicName.c_str()) != mode)
      continue;

    for (auto& diff : set._difficultyBeatmaps) {
      Rank rank = RankFromString(diff._difficulty.c_str());
      difficulties |= (1 << (int)rank);
    }

    break;
  }

  return difficulties;
}

int GetInfoFromDir(const char* dir, BeatmapInfo& info) {
  char path[256];
  sprintf(path, "%s/Info.dat", dir);
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    LOG_ERROR("Failed to open Info.dat for %s\n", dir);
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