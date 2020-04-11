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
  cpu_util_ = ((float)((current_cpu - previous_cpu) / sysconf(_SC_CLK_TCK)) /
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

long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

bool Process::operator<(Process const& a) const {
  // cpu_util_ is a cached value and may be out of date, but can't call method
  // due to const restrictions - not sure what best solution for this would be?
  // For the sake of sanity, fallback on pid if the cpu difference is neglible
  // This will show more recent processes for equal usage
  return a.cpu_util_ - cpu_util_ > 0.01 || pid_ < a.pid_;
}
