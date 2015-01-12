
typedef struct {
  time_t date;
  float humidity;
  float tempSHT1x;
  float tempDS3232;
  int potar;
} Record;

// ====> IMPORTANT Names should have exactly 6 characters each! <======
const int nbMeasures = 4;
char names[nbMeasures][20] = { "humid.", "t_SHT1", "t_3232", "analog:" };
