#include "utils.hpp"

using std::cout;
using std::string;
using std::vector;

int main(const int argc, const char* argv[]) {
  if (argc == 1) {
    cout << "Usage: 1. sxx mode [user@]host [command]\n"
            "  or   2. sxx mode file1 [[user@]host:]file2\n"
            "1. if mode is ssh, list, term or ssh-copy-id\n"
            "2. if mode is scp or rsync\n";
    return 0;
  }

  vector<string> args(argv + 1, argv + argc);
  type type(args[0]);

  if (type.id_ == ssh || type.id_ == list || type.id_ == term ||
      type.id_ == ssh_copy_id) {
    if (type.id_ == ssh && args.size() < 3) {
      cout << "Usage: sxx ssh [user@]host command\n";
    }

    string& host = args[1];
    const string& cmd = args.size() == 2 ? "" : args[2];
    const string usr = split_by_char(host, '@');
    const vector<string> hosts = get_hosts(host);
    grp_cmd(type, hosts, usr, cmd);
  } else {  // it's an scp or an rsync
    const string& file1 = args[1];
    string& file2 = args[2];
    string host = split_by_char(file2, ':');
    host.pop_back();
    file2 = ':' + file2;
    const string usr = split_by_char(host, '@');
    const vector<string> hosts = get_hosts(host);
    grp_cmd(type, hosts, usr, file1, file2);
  }

  return 0;
}
