#include "system.h"

#include <unistd.h>

#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() {
  std::set<int> current;
  for (int p : LinuxParser::Pids()) {
    current.emplace(p);
  }
  std::set<int> previous;
  for (Process p : processes_) {
    previous.emplace(p.Pid());
  }

  for (int i = processes_.size() - 1; i >= 0; i--) {
    if (current.find(processes_[i].Pid()) == current.end()) {
      // Order isn't important, so remove the item by replacing it with the
      // item at the back of the list and shrinking the list to avoid
      // reallocating the remainder of the vector
      processes_[i] = processes_.back();
      processes_.pop_back();
    }
  }

  for (int pid : current) {
    if (previous.find(pid) == previous.end()) {
      processes_.push_back(Process(pid));
    }
  }

  std::sort(processes_.rbegin(), processes_.rend());
  return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }