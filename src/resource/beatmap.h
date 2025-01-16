/**
 * Beatmap.h
 * Author: Beemer
 * Reflects the structure of the JSON inside of each ".dat" file
*/
#ifndef BEATMAP_H
#define BEATMAP_H
#include <gctypes.h>
#include <vector>
#include <memory>
#include <string>
#include "../json_struct.h"

JS_ENUM(Mode,
  Standard,
  OneSaber,
  ThreeSixty,
  Ninety,
  NoArrows,
  Lightshow,
  Lawless,
  Unknown
)

JS_ENUM(Modifier,
  ThreeSixty,
  Ninety,
  Unknown,
  NoArrows,
  Lightshow,
  Lawless,
  NoFail
)

JS_ENUM(Rank,
  Easy,
  Normal,
  Hard,
  Expert,
  ExpertPlus
)

struct BeatmapObject {
  float b;
  int x;
  int y;
};

JS_OBJ_EXT(BeatmapObject, b, x, y);


struct CustomDifficultyColor {
  float r, g, b;
};

JS_OBJ_EXT(CustomDifficultyColor, r, g, b);


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

JS_OBJ_EXT(CustomDifficultyData, _difficultyLabel, _requirements, _colorLeft,
  _colorRight, _envColorLeft, _envColorRight, _envColorWhite, _envColorLeftBoost,
  _envColorRightBoost, _envColorWhiteBoost, _obstacleColor);


struct DifficultyBeatmap {
   std::string _difficulty;
   int _difficultyRank;
   std::string _beatmapFilename;
   float _noteJumpMovementSpeed;
   float _noteJumpStartBeatOffset;

   CustomDifficultyData _customData;
};

JS_OBJ_EXT(DifficultyBeatmap, _difficulty, _difficultyRank, _beatmapFilename,
  _noteJumpMovementSpeed, _noteJumpStartBeatOffset, _customData);


struct DifficultyBeatmapSet {
  std::string _beatmapCharacteristicName;
  std::vector<DifficultyBeatmap> _difficultyBeatmaps;
};

JS_OBJ_EXT(DifficultyBeatmapSet, _beatmapCharacteristicName, _difficultyBeatmaps);

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

  int getModes();
  int getDifficulties(Mode mode);

  inline void reset() {
    _songName = "";
    _songSubName = "";
    _songAuthorName = "";
    _levelAuthorName = "";
    _beatsPerMinute = 0.0f;
    _shuffle = 0.0f;
    _shufflePeriod = 0.0f;
    _previewStartTime = 0.0f;
    _previewDuration = 0.0f;
    _songFilename = "";
    _coverImageFilename = "";
    _environmentName = "";
    _allDirectionsEnvironmentName = "";
    _songTimeOffset = 0.0f;
    _difficultyBeatmapSets.clear();
  }
};

JS_OBJ_EXT(BeatmapInfo, _songName, _songSubName, _songAuthorName,
  _levelAuthorName, _beatsPerMinute, _shuffle, _shufflePeriod,
  _previewStartTime, _previewDuration, _songFilename, _coverImageFilename,
  _environmentName, _allDirectionsEnvironmentName, _songTimeOffset,
  _difficultyBeatmapSets);

struct BeatmapBpmEvent {
  float b;
  float m;
};

JS_OBJ_EXT(BeatmapBpmEvent, b, m);


struct BeatmapRotationEvent {
  float b;
  int e;
  float r;
};

JS_OBJ_EXT(BeatmapRotationEvent, b, e, r);

struct BeatmapCustomNoteData {
  float angle;
};

JS_OBJ_EXT(BeatmapCustomNoteData, angle);


struct BeatmapColorNote {
  int c;
  int d;
  int a;

  BeatmapCustomNoteData customData;
};

JS_OBJ_EXT(BeatmapColorNote, c, d, a, customData);


struct BeatmapCustomObjectData {
  std::vector<float> coordinates; // array
};

JS_OBJ_EXT(BeatmapCustomObjectData, coordinates);


struct BeatmapBombNote {
  BeatmapCustomObjectData customData;
};

JS_OBJ_EXT(BeatmapBombNote, customData);

struct CustomObstacleData {
  std::vector<float> size; // Array
};

JS_OBJ_EXT(CustomObstacleData, size);

struct BeatmapObstacle {
  float d;
  int w;
  int h;

  CustomObstacleData customData;
};

JS_OBJ_EXT(BeatmapObstacle, d, w, h, customData);

struct BeatmapCustomSliderData {
  std::vector<float> tailCoordinates; // Array
};

JS_OBJ_EXT(BeatmapCustomSliderData, tailCoordinates);

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

  BeatmapCustomSliderData customData;
};

JS_OBJ_EXT(BeatmapSlider, c, d, mu, tb, tx, ty, tc, tmu, m, customData);

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

JS_OBJ_EXT(BeatmapBurstSlider, c, d, tb, tx, ty, sc, s, customData);

struct BeatmapBasicBeatmapEvent {
  float b;
  int et;
  int i;
  float f;
};

JS_OBJ_EXT(BeatmapBasicBeatmapEvent, b, et, i, f);

struct BeatmapColorBoostBeatmapEvent {
  float b;
  bool o;
};

JS_OBJ_EXT(BeatmapColorBoostBeatmapEvent, b, o);

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

JS_OBJ_EXT(BeatmapDifficulty, version, bpmEvents, rotationEvents, colorNotes,
  bombNotes, obstacles, sliders, burstSliders, basicBeatMapEvents,
  colorBoostBeatMapEvents, useNormalEventsAsCompatibleEvents);

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

JS_OBJ_EXT(Color, r, g, b, a);

struct NullableColorPalette {
  Color LeftNoteColor;
  Color RightNoteColor;
  Color LightColor1;
  Color LightColor2;
  Color WhiteLightColor;
  Color BoostLightColor1;
  Color BoostLightColor2;
  Color BoostWhiteLightColor;
  Color WallColor;
};

JS_OBJ_EXT(NullableColorPalette, LeftNoteColor, RightNoteColor, LightColor1,
  LightColor2, WhiteLightColor, BoostLightColor1, BoostLightColor2,
  BoostWhiteLightColor, WallColor);

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

JS_OBJ_EXT(Difficulty, mode, difficultyRank, beatmapDifficulty, NoteJumpSpeed,
  SpawnOffset, Label, requirements, colors);
JS_ENUM_DECLARE_STRING_PARSER(Mode);
JS_ENUM_DECLARE_STRING_PARSER(Rank);

typedef std::vector<Difficulty> DifficultyList;

extern int GetInfoFromDir(const char* dir, BeatmapInfo& info);

class Beatmap {
public:
  s32 voice;
  std::string directory;
  BeatmapInfo info;
  Difficulty map;

  Beatmap(std::string directory, BeatmapInfo info);
  int loadMap(Mode mode, Rank rank);

private:
  DifficultyBeatmapSet* getDifficultySet(Mode mode);

};

extern Mode ModeFromString(const char* name);
extern Rank RankFromString(const char* name);

typedef std::pair<std::string, BeatmapInfo> BeatmapPair;
typedef std::vector<BeatmapPair> BeatmapList;

#endif // BEATMAP_H