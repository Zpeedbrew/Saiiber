/**
 * Beatmap.h
 * Author: Beemer
 * Reflects the structure of the JSON inside of each ".dat" file
*/

#ifndef BEATMAP_H
#define BEATMAP_H
#include <gctypes.h>
#include <vector>
#include <string>

struct Track;

enum Mode {
  STANDARD,
  ONE_SABER,
  NO_ARROWS,
  THREE_SIXTY,
  NINETY,
  LIGHTSHOW,
  LAWLESS,
  UNKNOWN
};

enum Rank {
  EASY,
  NORMAL,
  HARD,
  EXPERT,
  EXPERT_PLUS
};

struct BeatmapObject {
  float b;
  int x;
  int y;
};

struct DifficultyBeatmapSet {
  std::string _beatmapCharacteristicName;
  std::vector<DifficultyBeatmap> _difficultyBeatmaps;
};

struct CustomDifficultyColor {
  float r, g, b;
};

struct CustomDifficultyData {
  std::string _difficultyLabel;
  std::vector<std::string> _requirements;

  CustomDifficultyColor _colorLeft;
  CustomDifficultyColor _colorRight;
  CustomDifficultyColor _envColorLeft;
  CustomDifficultyColor _envColorRight;
  CustomDifficultyColor _envColorWhite;
  CustomDifficultyColor _envColorLeftBoost;
  CustomDifficultyColor _envColorRightBoost;
  CustomDifficultyColor _envColorWhiteBoost;
  CustomDifficultyColor _obstacleColor;
};

struct CustomInfoData {};

struct DifficultyBeatmap {
   std::string _difficulty;
   int _difficultyRank;
   std::string _beatmapFilename;
   float _noteJumpMovementSpeed;
   float _noteJumpStartBeatOffset;

   CustomDifficultyData _customData;
};

struct BeatmapInfo {
  //Version doesn't need to be internally stored.
  std::string _songName;
  std::string _songSubName;
  std::string _songAuthorName;
  std::string _levelAuthorName;
  float _beatsPerMinute;

  //Shuffle is depreciated and will be ignored.
  float _shuffle;
  float _shufflePeriod;

  //Preview times aren't needed either
  float _previewStartTime;
  float _previewDuration;

  std::string _songFilename;
  std::string _coverImageFilename;
  std::string _environmentName;
  std::string _allDirectionsEnvironmentName;
  float _songTimeOffset;

  std::vector<DifficultyBeatmapSet> _difficultyBeatmapSets;
};

struct BeatmapBpmEvent {
  float b;
  float m;
};

struct BeatmapRotationEvent {
  float b;
  int e;
  float r;
};

struct BeatmapColorNote {
  int c;
  int d;
  int a;

  struct BeatmapCustomNoteData {
    // Angle isn't a thing in V3 noodle.
    // This just makes it easier to carry custom _cutDirection into V3
    float angle;
  } customData;
};

struct BeatmapCustomObjectData {
  float* coordinates; // array
};

struct BeatmapBombNote {
  BeatmapCustomObjectData customData;
};

struct BeatmapObstacle {
  float d;
  int w;
  int h;

  struct CustomObstacleData {
    float* size; // Array
  } customData;
};

struct BeatmapSlider : BeatmapObject {
  int c;
  int d;
  float mu;
  float tb;
  int tx;
  int ty;
  int tc;
  float tmu;
  int m;

  struct BeatmapCustomSliderData {
    float* tailCoordinates; // Array
  } customData;
};

struct BeatmapBurstSlider : BeatmapObject {
  int c;
  int d;
  float tb;
  int tx;
  int ty;
  int sc;
  float s;

  BeatmapCustomObjectData customData;
};

struct BeatmapBasicBeatmapEvent {
  float b;
  int et;
  int i;
  float f;
};

struct BeatmapColorBoostBeatmapEvent {
  float b;
  bool o;
};

struct BeatmapDifficulty {
  std::string version;
  std::vector<BeatmapBpmEvent> bpmEvents;
  std::vector<BeatmapRotationEvent> rotationEvents;
  std::vector<BeatmapColorNote> colorNotes;
  std::vector<BeatmapBombNote> bombNotes;
  std::vector<BeatmapObstacle> obstacles;
  std::vector<BeatmapSlider> sliders;
  std::vector<BeatmapBurstSlider> burstSliders;
  //Waypoints ommitted
  std::vector<BeatmapBasicBeatmapEvent> basicBeatMapEvents;
  std::vector<BeatmapColorBoostBeatmapEvent> colorBoostBeatMapEvents;
  bool useNormalEventsAsCompatibleEvents;
};

struct Color {
  float r, g, b, a;
  
  float& operator[] (int index) {
    switch (index) {
      case 0: return r;
      case 1: return g;
      case 2: return b;
      case 3: return a;
    }
  }
};

struct NullableColorPalette {
  Color* LeftNoteColor;
  Color* RightNoteColor;
  Color* LightColor1;
  Color* LightColor2;
  Color* WhiteLightColor;
  Color* BoostLightColor1;
  Color* BoostLightColor2;
  Color* BoostWhiteLightColor;
  Color* WallColor;
};

struct Difficulty {
  Mode mode;
  Rank difficultyRank;
  BeatmapDifficulty beatmapDifficulty;
  float NoteJumpSpeed;
  float SpawnOffset;
  std::string Label;
  std::vector<std::string> requirements;
  NullableColorPalette colors;
};

class LoadedMapData {
private:
  BeatmapInfo info;
  DifficultyList difficulties;

public:
    LoadedMapData(BeatmapInfo info, DifficultyList difficulties);
    LoadedMapData(const LoadedMapData& other) = default;
    LoadedMapData(LoadedMapData&& other) = default;
    ~LoadedMapData();

    inline BeatmapInfo getInfo() { return info; }
    inline DifficultyList getDifficulties() { return difficulties; }
};

class LoadedMap {
private:
  LoadedMapData map_data;
  Track* song;

public:
  LoadedMap(LoadedMapData&& mapData, char* coverImageData, Track* song);
  ~LoadedMap();

  inline LoadedMapData getMapData() { return map_data; }
  inline BeatmapInfo getInfo() { return map_data.getInfo(); }
  inline DifficultyList getDifficulties() { return map_data.getDifficulties(); }
  inline Track* getSong() { return song; }
};

extern LoadedMapData&& GetMapData(const char* directory);

#endif // BEATMAP_H