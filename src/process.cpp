#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid_; }

float Process::CpuUtilization() {
  long system_uptime = LinuxParser::UpTime();
  vector<int> values = LinuxParser::CpuUtilization(pid_);

  float total_time = values[0] + values[1] + values[2] + values[3];
  float seconds = system_uptime - values[4] / sysconf(_SC_CLK_TCK);
  cpu_util_ = 100.0 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
  return cpu_util_;
}

string Process::Command() {
  if (command_.empty()) {
    command_ = LinuxParser::Command(pid_);
  }
  return command_;
}

string Process::Ram() { return LinuxParser::Ram(pid_); }

string Process::User() {
  if (user_.empty()) {
    user_ = LinuxParser::User(pid_);
  }
  return user_;
}

long int Process::UpTime() {
  if (uptime_ == 0) {
    uptime_ = LinuxParser::UpTime(pid_);
  }
  return uptime_;
}

bool Process::operator<(Process const& a) const {
  return cpu_util_ < a.cpu_util_;
}
