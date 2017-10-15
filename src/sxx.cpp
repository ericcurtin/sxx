#include <vector>
#include <string>
#include <iostream>

using std::vector;
using std::string;

vector<string> toVector(const char* charv[]) {
  vector<string> ret;

  for (++charv; *charv; ++charv) {
    ret.push_back(*charv);
  }

  return ret;
}

int main(const int, const char* argv[]) {
  vector<string> argV = toVector(argv);
  string type = argV[0];

  if (type == "ssh" || type == "scp" || type == "rsync" || type == "term") {
    argV.erase(argV.begin());
  }
  else {
    type = "ssh";
  }

  for(vector<string>::const_iterator i = argV.begin(); i != argV.end(); ++i) {
    const string& thisStr = *i;
    if (thisStr[0] == '-') {
      continue;
    }

    size_t start = thisStr.find('@');
    if (start == string::npos) {
        start = 0;
    }
    else {
        ++start;
    }

    size_t end = thisStr.find(':', start);
    if (end == string::npos) {
        end = thisStr.size();
    }

    const string host_group = thisStr.substr(start, end - start);
    std::cout << host_group << "\n";
  }

}
