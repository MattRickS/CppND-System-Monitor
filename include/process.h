#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid) : pid_(pid) {}
  int Pid();
  std::string User();
  std::string Command();
  float CpuUtilization();
  std::string Ram();
  long int UpTime();
  bool operator<(Process const& a) const;

 private:
  int pid_;
  // Cached constant lifetime values
  std::string user_;
  std::string command_;
  // Cached for calculating cpu util over time
  long sys_uptime_{0};
  int cpu_total_{0};
  float cpu_util_{0.0f};
};

#endif