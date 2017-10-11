#include <vector>
#include <string>

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
  string type = argV[1];

  if (type == "ssh" || type == "scp" || type == "rsync" || type == "term") {
    argV.erase(argV.begin());
  }
  else {
    type == "ssh";
  }
}

