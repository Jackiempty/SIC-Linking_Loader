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
  Rec_type type;
  uint32_t address;
  uint32_t length;
  string objcode;
  string raw;
} OBJLINE;

class LLR {
 public:
  LLR();
  uint8_t* Loader(vector<vector<OBJLINE>> progs);
  vector<vector<OBJLINE>> loadOBJ(vector<string> file);
  void parseLine(string line, vector<OBJLINE>& prog);
  void insert(const string label, int address);
  void DisplayTable();
  bool findLabel(const string symbol, int& address);
  uint8_t get_mem(uint32_t address);
  void mem_Display();
  void dump(string file_name);

 private:
  unordered_map<string, int> table;
  uint8_t mem[0xFFFFFF];
  int length;
  string main;
};

#endif