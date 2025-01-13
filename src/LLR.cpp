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
      parseLine(line, prog);
    }
    PROGS.push_back(prog);
  }
  return PROGS;
}

void LLR::parseLine(string line, vector<OBJLINE>& prog) {
  // cout << "line: " << line << endl;
  OBJLINE objline;
  if (line[0] == 'H') {
    objline.type = Header;
    // cout << stoi(line.substr(7, 6)) << endl;
    objline.length = stoi(line.substr(13, 6), 0, 16);
    objline.address = stoi(line.substr(7, 6), 0, 16);
    string symbol = line.substr(1, 6);
    symbol.erase(remove_if(symbol.begin(), symbol.end(), ::isspace), symbol.end());
    insert(symbol, stoi(line.substr(7, 6), 0, 16));
    objline.length = stoi(line.substr(13), 0, 16);
    objline.raw = line;
    prog.push_back(objline);
  } else if (line[0] == 'T') {
    objline.type = Text;
    objline.address = stoi(line.substr(1, 6), 0, 16);
    objline.length = stoi(line.substr(7, 2), 0, 16);
    objline.objcode = line.substr(9);
    objline.raw = line;
    prog.push_back(objline);
  } else if (line[0] == 'M') {
    objline.type = Modification;
    objline.address = stoi(line.substr(1, 6), 0, 16);
    objline.length = stoi(line.substr(7, 2), 0, 16);
    objline.objcode = line.substr(9);
    objline.raw = line;
    prog.push_back(objline);
  } else if (line[0] == 'D') {
    int i = 1;
    objline.type = Define;
    while (line[i]) {
      // cout << line.substr(i + 6, 6) << endl;
      string symbol = line.substr(i, 6);
      symbol.erase(remove_if(symbol.begin(), symbol.end(), ::isspace), symbol.end());
      insert(symbol, stoi(line.substr(i + 6, 6), 0, 16));
      objline.address = stoi(line.substr(i + 6, 6), 0, 16);
      objline.raw = line.substr(i, 12);
      prog.push_back(objline);
      i += 12;
    }
  } else if (line[0] == 'R') {
    objline.type = Refer;
    objline.raw = line;
    prog.push_back(objline);
  } else if (line[0] == 'E') {
    objline.type = End;
    objline.raw = line;
    prog.push_back(objline);
  } else {
    throw runtime_error("Type not found: " + line.substr(0, 1));
  }
}

uint8_t* LLR::Loader(vector<vector<OBJLINE>> progs) {
  int start = PROGADDR;
  int tmp_length;
  for (int i = 0; i < progs.size(); i++) {
    for (int j = 0; j < progs[i].size(); j++) {
      if (progs[i][j].type == Header) {
        string symbol = progs[i][j].raw.substr(1, 6);
        progs[i][j].address += start;
        if (progs[i][j].address == PROGADDR) {
          main = symbol;
        }
        symbol.erase(remove_if(symbol.begin(), symbol.end(), ::isspace), symbol.end());
        table[symbol] = start;
        tmp_length = progs[i][j].length;
        // start += progs[i][j].length;
        // cout << progs[i][j].raw << " : " << int_to_hex(progs[i][j].address, 6) << ", "
        //      << int_to_hex(table[progs[i][j].raw.substr(1, 6)], 6) << endl;
      } else if (progs[i][j].type == Define) {
        int line_start = start + progs[i][j].address;
        string symbol = progs[i][j].raw.substr(0, 6);
        symbol.erase(remove_if(symbol.begin(), symbol.end(), ::isspace), symbol.end());
        table[symbol] = line_start;
      }
    }
    start += tmp_length;
  }
  for (int i = 0; i < progs.size(); i++) {
    for (int j = 0; j < progs[i].size(); j++) {
      switch (progs[i][j].type) {
        case (Header): {
          start = progs[i][j].address;
          length += progs[i][j].length;
        } break;
        case (Text): {
          int line_start = start + progs[i][j].address;
          for (int k = 0; k < progs[i][j].length; k++) {
            mem[line_start] = stoi(progs[i][j].objcode.substr(k * 2, 2), 0, 16);
            line_start += 1;
          }
        } break;
        case (Modification): {
          int line_start = start + progs[i][j].address;
          int sum;
          if (progs[i][j].length == 5) {
            sum = ((mem[line_start] & 0b00001111) << 16) + (mem[line_start + 1] << 4) + mem[line_start + 2];
          } else if (progs[i][j].length == 6) {
            sum = (mem[line_start] << 16) + (mem[line_start + 1] << 4) + mem[line_start + 2];
          }
          string symbol = progs[i][j].raw.substr(10, 6);
          symbol.erase(remove_if(symbol.begin(), symbol.end(), ::isspace), symbol.end());
          int shift = table[symbol];  // shift ok, sum ok
          // cout << setw(6) << progs[i][j].raw.substr(10, 6) << ": " << int_to_hex(sum, 6) << endl;
          if (progs[i][j].raw.substr(9, 1) == "+") {
            sum += shift;
          } else if (progs[i][j].raw.substr(9, 1) == "-") {
            sum -= shift;
          }
          if (progs[i][j].length == 5) {
            int temp = (mem[line_start] & 0b11110000) << 16;
            cout << int_to_hex(line_start, 6) << ", tmp: " << int_to_hex(temp, 6) << endl;
            sum += temp;
            string sum_str = int_to_hex(sum, 6);
            for (int i = 0; i < 3; i++) {
              mem[line_start + i] = stoi(sum_str.substr(i * 2, 2), 0, 16);
            }
          } else if (progs[i][j].length == 6) {
            string sum_str = int_to_hex(sum, 6);
            cout << int_to_hex(line_start, 6) << ": " << sum_str << endl;
            for (int i = 0; i < 3; i++) {
              mem[line_start + i] = stoi(sum_str.substr(i * 2, 2), 0, 16);
            }
          }
        } break;
        case(Define):
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
    cout << "Name: " << entry.first << ", Address: " << int_to_hex(entry.second, 6) << endl;
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
  for (int i = 0x1000; i < 0x10C0; i++) {
    if ((i - 0x1000) % 8 == 0) cout << int_to_hex(i, 6) << ": ";
    cout << int_to_hex(mem[i], 2);
    cout << " ";
    if ((i - 0x0FFF) % 8 == 0) cout << endl;
  }
}

void LLR::dump(string file_name) {
  string output = "./test/" + file_name;
  cout << output << endl;
  ofstream imgf(output);
  string obj_line = "";
  string obj_line_tmp = "";
  int locctr;
  imgf << "H" << main << int_to_hex(PROGADDR, 6) << int_to_hex(length, 6) << endl;
  for (int i = 0; i < length; i++) {
    obj_line_tmp = obj_line + int_to_hex(mem[PROGADDR + i], 2);
    if (obj_line_tmp.length() > 60) {
      locctr = PROGADDR + i - (obj_line.length() / 2);
      imgf << "T" << int_to_hex(locctr, 6) << int_to_hex(obj_line.length() / 2, 2) << obj_line << endl;
      obj_line = "";
      obj_line += int_to_hex(mem[PROGADDR + i], 2);
    } else {
      obj_line = obj_line_tmp;
    }
  }
  locctr = PROGADDR + length - (obj_line.length() / 2);
  imgf << "T" << int_to_hex(locctr, 6) << int_to_hex(obj_line.length() / 2, 2) << obj_line << endl;
  imgf << "E" << int_to_hex(PROGADDR, 6) << endl;

  imgf.close();
}