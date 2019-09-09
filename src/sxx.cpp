#include "utils.hpp"

using std::string;
using std::vector;
using std::map;

int main(const int argc, const char* argv[]) {
  if (argc == 1) {
    printf("Usage: 1. sxx mode [[user@]host] [command]\n"
           "  or   2. sxx mode file1 [[user@]host]file2\n"
           "1. if mode is ssh, list, term or ssh-copy-id\n"
           "2. if mode is scp or rsync\n");
    return 0;
  }

  vector<string> args(argv + 1, argv + argc);
  type type(args[0]);

  printf("%s:%d\n", __FILE__, __LINE__);
  if (type.id_ == ssh || type.id_ == list || type.id_ == term ||
      type.id_ == ssh_copy_id || type.id_ == exe) {
        printf("%s:%d\n", __FILE__, __LINE__);
    if ((type.id_ == ssh || type.id_ == exe) && args.size() < 3) {
      const string last_arg = type.id_ == ssh ? "command" : "script";
      printf("Usage: sxx %s [user@]host %s\n", type.name_.c_str(),
             last_arg.c_str());
      return 0;
    }
      printf("%s:%d\n", __FILE__, __LINE__);

    string host = args.size() < 2 ? "" : args[1];
      printf("%s:%d\n", __FILE__, __LINE__);
    const string& cmd = args.size() < 3 ? "" : args[2];
      printf("'%s' %s:%d\n", host.c_str(), __FILE__, __LINE__);
    const string usr = split_by_char(host, '@');
      printf("%s:%d\n", __FILE__, __LINE__);
    const map<string, string> hosts = get_hosts(host);
      printf("%s:%d\n", __FILE__, __LINE__);
    grp_cmd(type, hosts, usr, cmd);
  } else {  // it's an scp or an rsync
    const string& file1 = args[1];
    string& file2 = args[2];
    string host = split_by_char(file2, ':');
    host.pop_back();
    file2 = ':' + file2;
    const string usr = split_by_char(host, '@');
    const map<string, string> hosts = get_hosts(host);
      printf("%s:%d\n", __FILE__, __LINE__);
    grp_cmd(type, hosts, usr, file1, file2);
  }

  return 0;
}
