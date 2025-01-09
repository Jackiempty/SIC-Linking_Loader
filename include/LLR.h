#ifndef LLR_H
#define LLR_H

#define PROGADDR 4096

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
using namespace std;

typedef enum { Header, Text, Modification, Define, Refer, End } Rec_type;

typedef struct {
  string name;
  uint32_t address;
  string control_sec;
} ESTAB;

typedef struct {
  Rec_type type;
  uint32_t address;
  uint32_t length;
  string objcode;
  string raw;
} OBJLINE;

class memory {
 public:
  memory();
  uint8_t get_mem(uint32_t address);

 private:
  uint8_t mem[0xFFFFFF];
};

class LLR {
 public:
  LLR();
  memory Loader(vector<vector<OBJLINE>> progs);
  vector<vector<OBJLINE>> loadOBJ(vector<string> file);
  OBJLINE parseLine(string line);
  void insert(const string label, int address);
  void DisplayTable();
  bool findLabel(const string symbol, int &address);

 private:
  unordered_map<string, int> table;
};

#endif