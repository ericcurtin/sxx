#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <fstream>

#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/StreamCopier.h>

using std::vector;
using std::string;
using std::ifstream;
using std::cout;
using std::cerr;
using std::regex;
using std::regex_match;

using Poco::Pipe;
using Poco::Process;
using Poco::ProcessHandle;

vector<string> get_hosts(const string& host_grp) {
  ifstream is("/etc/sxx/hosts");
  vector<string> hosts;
  for (string line; getline(is, line);) {
    if (line[0] != '[') {
      continue;
    }

    const size_t end = line.find(']');
    const string cur_host_grp = line.substr(1, end - 1);
    if (regex_match(cur_host_grp, regex(host_grp))) {
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

struct proc {
  const string host_;
  Pipe out_pipe_;
  const ProcessHandle proc_hand_;

  proc(const string& host, const vector<string>& args)
      : host_(host),
        proc_hand_(Process::launch(args[0], args, 0, &out_pipe_, 0)) {}
};

void grpCmd(const string& type,
            const string& user,
            const vector<string>& hosts,
            const string& cmd) {
  vector<proc> procs;
  for (vector<string>::const_iterator i = hosts.begin(); i != hosts.end();
       ++i) {
    const vector<string> args = {type, user + '@' + *i, cmd};
    proc proc(type, args);
    procs.push_back(proc);
  }

  for (vector<proc>::const_iterator i = procs.begin(); i != procs.end(); ++i) {
    const proc& proc = *i;
    Poco::PipeInputStream i_str(proc.out_pipe_);
    string out;
    for (int c = i_str.get(); c != -1; c = i_str.get()) {
      out += static_cast<char>(c);
    }

    const int es = proc.proc_hand_.wait();
    const string color = es ? "\033[;31m" : "\033[;32m";
    cout << color << proc.host_ << " | "
         << "es=" << es << "\033[m\n"
         << out << "\n\n";
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

    const string host_grp = this_str.substr(at_pos, end - at_pos);
    const vector<string> hosts = get_hosts(host_grp);
    grpCmd(type, user, hosts, *(++i));
    return 0;
  }

  return 1;
}
