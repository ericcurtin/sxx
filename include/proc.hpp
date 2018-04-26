#ifndef PROC_HPP
#define PROC_HPP

#include <Poco/PipeStream.h>
#include <Poco/Process.h>

struct proc {
  const std::string host_;
  Poco::Pipe out_pipe_;
  Poco::Pipe err_pipe_;
  const Poco::ProcessHandle proc_hand_;

  proc(const std::string& host,
       const std::string& command,
       const std::vector<std::string>& args)
      : host_(host),
        proc_hand_(Poco::Process::launch(command,
                                         args,
                                         nullptr,
                                         &out_pipe_,
                                         &err_pipe_)) {}
};

#endif
