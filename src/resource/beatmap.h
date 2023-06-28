/**
 * Beatmap.h
 * Author: Beemer
 * Reflects the structure of the JSON inside of each ".dat" file
*/

#ifndef BEATMAPH
#define BEATMAPH
#include <gctypes.h>
#include <vector>

struct Track;

enum DifficultyCharacteristic {
  STANDARD,
  ONE_SABER,
  NO_ARROWS,
  THREE_SIXTY,
  NINETY,
  LIGHTSHOW,
  LAWLESS,
  UNKNOWN
};

enum DifficultyRank {
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
  const char* beatmapCharacteristicName;
  DifficultyBeatmap difficultyBeatmaps[];
};

struct CustomDifficultyColor {
  float r;
  float g;
  float b;
};

struct CustomDifficultyData {
  const char* difficultyLabel;
  const char** requirements;

  CustomDifficultyColor colorLeft;
  CustomDifficultyColor colorRight;
  CustomDifficultyColor envColorLeft;
  CustomDifficultyColor envColorRight;
  CustomDifficultyColor envColorWhite;
  CustomDifficultyColor envColorLeftBoost;
  CustomDifficultyColor envColorRightBoost;
  CustomDifficultyColor envColorWhiteBoost;
  CustomDifficultyColor obstacleColor;
};

struct CustomInfoData {};

struct DifficultyBeatmap {
   const char* difficulty;
   int difficultyRank;
   const char* beatmapFilename;
   float noteJumpMovementSpeed;
   float noteJumpStartBeatOffset;

   CustomDifficultyData customData;
};

struct BeatmapInfo {
  //Version doesn't need to be internally stored.
  const char* songName;
  const char* songSubName;
  const char* songAuthorName;
  const char* levelAuthorName;
  float beatsPerMinute;
  //Shuffle is depreciated and will be ignored.
  float shuffle;
  float shufflePeriod;
  //Preview times aren't needed either
  float previewStartTime;
  float previewDuration;
  const char* songFilename;
  const char* coverImageFilename;
  const char* environmentName;
  const char* allDirectionsEnvironmentName;
  float songTimeOffset;

  CustomInfoData customData;

  DifficultyBeatmapSet difficultyBeatmapSets[];
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
    //Angle isn't a thing in V3 noodle. This just makes it easier to carry custom _cutDirection into V3
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
  const char* version;
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
  DifficultyCharacteristic characteristic;
  DifficultyRank difficultyRank;
  BeatmapDifficulty beatmapDifficulty;
  float NoteJumpSpeed;
  float SpawnOffset;
  const char* Label;
  const char** requirements;
  NullableColorPalette colors;
};

class LoadedMapData {
private:
  BeatmapInfo info;
  std::vector<Difficulty> difficulties;

public:
    LoadedMapData(BeatmapInfo info, std::vector<Difficulty> difficulties);
    LoadedMapData(const LoadedMapData& other) = default;
    LoadedMapData(LoadedMapData&& other) = default;
    ~LoadedMapData();

    inline BeatmapInfo getInfo() { return info; }
    inline std::vector<Difficulty> getDifficulties() { return difficulties; }
};

class LoadedMap {
private:
  LoadedMapData map_data;
  Track* song;

public:
  LoadedMap(LoadedMapData& mapData, char* coverImageData, Track* song);
  ~LoadedMap();

  inline LoadedMapData getMapData() { return map_data; }
  inline BeatmapInfo getInfo() { return map_data.getInfo(); }
  inline std::vector<Difficulty> getDifficulties() { return map_data.getDifficulties(); }
  inline Track* getSong() { return song; }
};

#endif // BEATMAPH



const BeatmapInfo BEATMAP_EMPTY = {
  "",                 // songName
  "",                 // songSubName
  "",                 // songAuthorName
  "",                 // levelAuthorName
  120,                // beatsPerMinute
  0,                  // shuffle
  0,                  // shufflePeriod
  0,                  // previewStartTime
  0,                  // previewDuration
  "",                 // songFilename
  "",                 // coverImageFilename
  "",                 // environmentName
  "",                 // allDirectionsEnvironmentName
  0,                  // songTimeOffset
  CustomInfoData(),   // customData
  { }                 // difficultyBeatmapSets
};