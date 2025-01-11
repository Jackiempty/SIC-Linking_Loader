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
      // cout << line << endl;
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
    // cout << stoi(line.substr(7, 6)) << endl;
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
      // cout << line.substr(i + 6, 6) << endl;
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

uint8_t* LLR::Loader(vector<vector<OBJLINE>> progs) {
  int start = PROGADDR;
  for (int i = 0; i < progs.size(); i++) {
    for (int j = 0; j < progs[i].size(); j++) {
      if (progs[i][j].type == Header) {
        progs[i][j].address += start;
        table[progs[i][j].raw.substr(1, 6)] = start;
        start += progs[i][j].length;
        // cout << progs[i][j].raw << " : " << int_to_hex(progs[i][j].address, 6) << ", "
        //      << int_to_hex(table[progs[i][j].raw.substr(1, 6)], 6) << endl;
      }
    }
  }
  for (int i = 0; i < progs.size(); i++) {
    for (int j = 0; j < progs[i].size(); j++) {
      switch (progs[i][j].type) {
        case (Header): {
          start = progs[i][j].address;
        } break;
        case (Text): {
          int line_start = start + progs[i][j].address;
          for (int k = 0; k < progs[i][j].length; k++) {
            mem[line_start] = stoi(progs[i][j].objcode.substr(k * 2, 2), 0, 16);
            line_start += 1;
          }

        } break;
        case (Modification): {
        } break;
        case (Define):
        case (Refer):
        case (End):
          break;
        default:
          cout << "No Record type found" << endl;
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

uint8_t LLR::get_mem(uint32_t address) { return mem[address]; }

void LLR::mem_Display() {
  for (int i = 0x1000; i < 0x2100; i++) {
    if ((i - 0x1000) % 8 == 0) cout << int_to_hex(i, 6) << ": ";
    cout << int_to_hex(mem[i], 2);
    cout << " ";
    if ((i - 0x0FFF) % 8 == 0) cout << endl;
  }
}