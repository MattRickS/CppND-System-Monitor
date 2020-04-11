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
  // Value in seconds
  long previous_uptime = sys_uptime_;
  sys_uptime_ = LinuxParser::UpTime();

  // Values in seconds: [utime, stime, cutime, cstime]
  int previous_cpu = cpu_total_;
  vector<int> values = LinuxParser::CpuUtilization(pid_);
  int current_cpu = values[0] + values[1] + values[2] + values[3];

  // Store cpu util for operator<
  cpu_util_ = (((float)(current_cpu - previous_cpu) / sysconf(_SC_CLK_TCK)) /
               (float)(sys_uptime_ - previous_uptime));
  cpu_total_ = current_cpu;
  // Note: ncurses display is converting to percentage, no need for multiplier
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
  // TODO: Always comparing with an out of date value, but can't call method
  // due to const restrictions
  return cpu_util_ < a.cpu_util_;
}
