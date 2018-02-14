#include <iostream>
#include <regex>
#include <fstream>

#include <Poco/Process.h>
#include <Poco/PipeStream.h>
#include <Poco/String.h>
#include <Poco/Glob.h>

using std::vector;
using std::string;
using std::ifstream;
using std::cout;

using Poco::Pipe;
using Poco::Process;
using Poco::ProcessHandle;
using Poco::trim;
using Poco::Glob;

vector<string> get_hosts(const string& host_grp) {
  ifstream is("/etc/sxx/hosts");
  vector<string> hosts;
  for (char c; is.get(c);) {
    if (c == '#') {
      while (is.get(c) && c != '\n') {
      }
    }

    if (c == '[') {
      string cur_host_grp;
      for (; is.get(c) && c != ']' && c != '\n'; cur_host_grp += c) {
      }
      trim(cur_host_grp);

      Glob glob(host_grp);
      if (!glob.match(cur_host_grp)) {
        continue;
      }

      for (string host; is.get(c); host += c) {
        if (c == '[') {
          is.unget();
          break;
        }

        if (isspace(c)) {
          trim(host);
          host.erase(remove(host.begin(), host.end(), '\n'), host.end());
          if (!host.empty()) {
            hosts.push_back(host);
            host.clear();
            continue;
          }
        }
      }
    }
  }

  return hosts;
}

struct proc {
  const string host_;
  Pipe out_pipe_;
  const ProcessHandle proc_hand_;

  proc(const string& host, const string& type, const vector<string>& args)
      : host_(host),
        proc_hand_(Process::launch(type, args, nullptr, &out_pipe_, nullptr)) {}
};

void grp_cmd(const string& type,
             const string& user,
             const vector<string>& hosts,
             const string& cmd) {
  if (type == "list") {
    for (const string& host : hosts) {
      cout << host << '\n';
    }
    return;
  }

  vector<proc> procs;
  for (const string& host : hosts) {
    const vector<string> args = {user + host, cmd};
    proc proc(host, type, args);
    procs.push_back(proc);
  }

  for (const proc& proc : procs) {
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

  if (type == "ssh" || type == "scp" || type == "rsync" || type == "term" ||
      type == "list") {
    args.erase(args.begin());
  } else {
    type = "ssh";
  }

  for (auto i = args.begin(); i != args.end(); ++i) {
    const string& arg = *i;
    if (arg[0] == '-') {
      continue;
    }

    size_t at_pos = arg.find('@');
    string user;
    if (at_pos == string::npos) {
      at_pos = 0;
    } else {
      user = arg.substr(0, at_pos) + '@';
      ++at_pos;
    }

    const size_t end = arg.size();

    const string host_grp = arg.substr(at_pos, end - at_pos);
    const vector<string> hosts = get_hosts(host_grp);
    grp_cmd(type, user, hosts, *(++i));
    return 0;
  }

  return 1;
}
