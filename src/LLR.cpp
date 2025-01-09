#include "LLR.h"
using namespace std;

static inline string int_to_hex(int i, int space) {
  stringstream stream;
  stream << setfill('0') << setw(space) << hex << uppercase << i;
  return stream.str().substr(stream.str().length() - space);
}

LLR::LLR() {}

vector<vector<OBJLINE>> LLR::loadOBJ(vector<string> file) {
  vector<vector<OBJLINE>> PROGS;
  for (int i = 0; i < file.size(); i++) {
    vector<OBJLINE> prog;
    string line;
    ifstream fin(file[i]);
    if (!fin) throw runtime_error("[!] Cannot open input file:" + file[i]);
    while (getline(fin, line)) {
      cout << line << endl;
      prog.push_back(parseLine(line));
    }
    PROGS.push_back(prog);
  }
  return PROGS;
}

OBJLINE LLR::parseLine(string line) {
  // cout << "line: " << line << endl;
  OBJLINE objline;
  objline.raw = line;
  if (line[0] == 'H') {
    objline.type = Header;
    cout << stoi(line.substr(7, 6)) << endl;
    objline.length = stoi(line.substr(13, 6), 0, 16);
    objline.address = stoi(line.substr(7, 6), 0, 16);
    insert(line.substr(1, 6), stoi(line.substr(7, 6), 0, 16));
    objline.length = stoi(line.substr(13), 0, 16);
  } else if (line[0] == 'T') {
    objline.type = Text;
    objline.address = stoi(line.substr(1, 6), 0, 16);
    objline.length = stoi(line.substr(7, 2), 0, 16);
    objline.objcode = line.substr(9);
  } else if (line[0] == 'M') {
    objline.type = Modification;
    uint8_t address = stoi(line.substr(1, 6), 0, 16);
    objline.length = stoi(line.substr(7, 2), 0, 16);
    objline.objcode = line.substr(9);

  } else if (line[0] == 'D') {
    int i = 1;
    objline.type = Define;
    while (line[i]) {
      cout << line.substr(i + 6, 6) << endl;
      insert(line.substr(i, 6), stoi(line.substr(i + 6, 6), 0, 16));
      i += 12;
    }
  } else if (line[0] == 'R') {
    objline.type = Refer;
  } else if (line[0] == 'E') {
    objline.type = End;
  } else {
    throw runtime_error("Type not found: " + line.substr(0, 1));
  }
  return objline;
}

memory LLR::Loader(vector<vector<OBJLINE>> progs) {
  memory mem;
  int start = PROGADDR;
  for (int i = 0; i < progs.size(); i++) {
    for (int j = 0; j < progs[i].size(); j++) {
      if (progs[i][j].type == Header) {
        progs[i][j].address += start;
        start += progs[i][j].length;
        cout << progs[i][j].raw << " : " << int_to_hex(progs[i][j].address, 6) << endl;
      }
    }
  }
  return mem;
}

void LLR::insert(const string symbol, int address) { table[symbol] = {address}; }

void LLR::DisplayTable() {
  cout << "Symbol Table" << endl;
  for (const auto& entry : table) {
    cout << "Name: " << entry.first << " , Address: " << entry.second;
  }
}

bool LLR::findLabel(const string symbol, int& address) {
  auto it = table.find(symbol);
  if (it != table.end()) {
    address = it->second;
    return true;
  }
  return false;
}

memory::memory() { cout << "24 byte memory allocated" << endl; }

uint8_t memory::get_mem(uint32_t address) { return mem[address]; }