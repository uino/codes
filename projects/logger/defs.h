

const int nbMeasures = 3; // --currently, at most 5 measures

typedef struct {
  time_t date;
  float values[nbMeasures];
} Record;

typedef const char* fstring;

typedef struct {
  int nbItems;
  const fstring* items;
} PanelDescr;
