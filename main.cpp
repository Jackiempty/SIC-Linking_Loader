#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "LLR.h"
using namespace std;

int main(int argc, char* argv[]) {
  // for (int i = 0; i < argc; i++) cout << argv[i] << endl;
  const string& fn = argv[1];
  string output = "imgf";
  string report = "rptf";
  vector<string> input;
  for (int i = 1; i < argc; i++) {
    if ((string)argv[i] == "-o") {
      output = argv[i + 1];
      i++;
    } else if ((string)argv[i] == "-r") {
      report = argv[i + 1];
      i++;
    } else {
      input.push_back(argv[i]);
    }
  }
  vector<vector<OBJLINE>> PROGS;
  uint8_t* mem;
  LLR Llr;
  PROGS = Llr.loadOBJ(input);
  mem = Llr.Loader(PROGS);
  Llr.mem_Display();
  return 0;
}
