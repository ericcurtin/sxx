#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <iostream>
#include <fstream>

using std::vector;
using std::string;
using std::ifstream;
using std::cout;
using std::cerr;

vector<string> get_hosts(const string& hostGrp) {
  ifstream is("/etc/sxx/hosts");
  vector<string> hosts;
  for (string line; getline(is, line);) {
    if (line[0] != '[') {
      continue;
    }

    const size_t end = line.find(']');
    const string curHostGrp = line.substr(1, end - 1);
    if (curHostGrp != hostGrp) {
      continue;
    }

    for (string host; getline(is, host);) {
      if (host[0] == '[') {
        return hosts;
      }

      if (!host.empty()) {
        hosts.push_back(host);
      }
    }
  }

  return hosts;
}

enum PIPE_FILE_DESCRIPTERS { READ_FD = 0, WRITE_FD = 1 };

struct proc {
  int pid;
  static const int c2p_size = 2;
  int c2p[c2p_size];

  proc() : pid() {
    if (pipe(c2p) == -1) {
      cerr << "err: pipe " << EXIT_FAILURE << '\n';
    }
    pid = fork();
  }
};

void grpCmd(const char* type,
            const string& user,
            const vector<string>& hosts,
            const string& cmd) {
  vector<proc> procs;
  for (vector<string>::const_iterator i = hosts.begin(); i != hosts.end();
       ++i) {
    const string& host = *i;
    const proc proc1 = proc();
    procs.push_back(proc1);
    if (!proc1.pid) {
      dup2(proc1.c2p[WRITE_FD], STDOUT_FILENO);
      close(proc1.c2p[READ_FD]);
      close(proc1.c2p[WRITE_FD]);
      execlp(type, type, const_cast<char*>(string(user + host).c_str()),
             const_cast<char*>(cmd.c_str()), NULL);
    }
  }

  for (vector<proc>::const_iterator i = procs.begin(); i != procs.end(); ++i) {
    const proc& proc = *i;
    close(proc.c2p[WRITE_FD]);
    const ssize_t bytes_at_a_time = 80;
    char* read_buf = NULL;
    size_t buf_size = 0;
    size_t buf_offset = 0;
    while (1) {
      if (buf_offset + bytes_at_a_time > buf_size) {
        buf_size = bytes_at_a_time + buf_size * 2;
        read_buf = static_cast<char*>(realloc(read_buf, buf_size));
      }
      const ssize_t chars_io =
          read(proc.c2p[WRITE_FD], read_buf + buf_offset, bytes_at_a_time);
      if (chars_io <= 0) {
        break;
      }
      buf_offset += static_cast<size_t>(chars_io);
    }
    int es;
    waitpid(proc.pid, &es, 0);
    cout << read_buf;
  }
}

int main(const int argc, const char* argv[]) {
  vector<string> args(argv + 1, argv + argc);

  string type = args.empty() ? "" : args[0];

  if (type == "ssh" || type == "scp" || type == "rsync" || type == "term") {
    args.erase(args.begin());
  } else {
    type = "ssh";
  }

  for (vector<string>::const_iterator i = args.begin(); i != args.end(); ++i) {
    const string& this_str = *i;
    if (this_str[0] == '-') {
      continue;
    }

    size_t at_pos = this_str.find('@');
    string user;
    if (at_pos == string::npos) {
      at_pos = 0;
    } else {
      user = this_str.substr(0, at_pos) + '@';
      ++at_pos;
    }

    const size_t end = this_str.size();

    const string hostGrp = this_str.substr(at_pos, end - at_pos);
    const vector<string> hosts = get_hosts(hostGrp);
    grpCmd(type.c_str(), user, hosts, *(++i));
    return 0;
  }

  return 1;
}
