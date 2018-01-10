#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <vector>
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
  string host_;
  int pid_;
  int c2p_[2];

  proc(const string& host) : host_(host), pid_() {
    if (pipe(c2p_) == -1) {
      cerr << "err: pipe " << EXIT_FAILURE << '\n';
    }
    pid_ = fork();
  }
};

void grpCmd(const char* type,
            const string& user,
            const vector<string>& hosts,
            const string& cmd) {
  vector<proc> procs;
  for (vector<string>::const_iterator i = hosts.begin(); i != hosts.end();
       ++i) {
    const proc proc(*i);
    procs.push_back(proc);
    if (!proc.pid_) {
      dup2(proc.c2p_[WRITE_FD], STDOUT_FILENO);
      close(proc.c2p_[READ_FD]);
      close(proc.c2p_[WRITE_FD]);
      execlp(type, type, const_cast<char*>(string(user + proc.host_).c_str()),
             const_cast<char*>(cmd.c_str()), NULL);
    }
    close(proc.c2p_[WRITE_FD]);
  }

  for (vector<proc>::const_iterator i = procs.begin(); i != procs.end(); ++i) {
    const proc& proc = *i;
    char buf[1024];
    string out;
    for (ssize_t s; (s = read(proc.c2p_[READ_FD], buf, sizeof(buf)));) {
      out += buf;
    }

    int es;
    waitpid(proc.pid_, &es, 0);
    string color = es ? "\033[;31m" : "\033[;32m";
    cout << color << proc.host_ << " | "
         << "es=" << WEXITSTATUS(es) << "\033[m\n"
         << buf << "\n\n";
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
