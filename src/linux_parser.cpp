#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

float LinuxParser::MemoryUtilization() {
  float total, free;
  string temp, line;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream first_linestream(line);
    first_linestream >> temp >> total;
    std::getline(stream, line);
    std::istringstream second_linestream(line);
    second_linestream >> temp >> free;
  }
  return (total - free) / total;
}

long LinuxParser::UpTime() {
  long active_uptime_seconds;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> active_uptime_seconds;
  }
  return active_uptime_seconds;
}

vector<float> LinuxParser::CpuUtilization() {
  // user nice system idle iowait irq softirq steal guest guest_nice
  vector<float> values(10);
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    // Discard the first value ("cpu")
    linestream.ignore(256, ' ');
    for (int i = 0; i < 10; i++) {
      linestream >> values[i];
    }
  }
  return values;
}

std::istringstream LinuxParser::FindLineStream(string filepath,
                                               string findKey) {
  string key = "";
  string line;
  std::ifstream stream(filepath);
  std::istringstream linestream;
  if (stream.is_open()) {
    while (!stream.eof()) {
      std::getline(stream, line);
      linestream.str(line);
      linestream >> key;
      if (key == findKey) {
        break;
      }
    }
  }
  return linestream;
}

int LinuxParser::TotalProcesses() {
  int total;
  std::istringstream s =
      FindLineStream(kProcDirectory + kStatFilename, "processes");
  s >> total;
  return total;
}

int LinuxParser::RunningProcesses() {
  int running;
  std::istringstream s =
      FindLineStream(kProcDirectory + kStatFilename, "procs_running");
  s >> running;
  return running;
}

string LinuxParser::Command(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    string cmdPath;
    std::getline(stream, cmdPath, ' ');
    int index = cmdPath.rfind('/', cmdPath.length());
    if (index != string::npos) {
      return cmdPath.substr(index + 1, cmdPath.length() - 1);
    }
  }
  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid [[maybe_unused]]) { return string(); }

int LinuxParser::Uid(int pid) {
  string filepath = kProcDirectory + std::to_string(pid) + kStatusFilename;
  std::istringstream s = FindLineStream(filepath, "Uid:");
  int uid;
  s >> uid;
  return uid;
}

string LinuxParser::User(int pid) {
  int uid = Uid(pid);
  // TODO: Some form of caching should happen here - uid -> name doesn't change
  string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string name, tmp;
    int userid;

    while (!stream.eof()) {
      std::getline(stream, line);
      std::stringstream linestream(line);
      std::getline(linestream, name, ':');
      std::getline(linestream, tmp, ':');
      std::getline(linestream, tmp, ':');
      std::stringstream(tmp) >> userid;
      if (userid == uid) {
        return name;
      }
    }
  }

  return string();
}

long LinuxParser::UpTime(int pid) {
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  long uptime;
  if (stream.is_open()) {
    string line, tmp;
    std::getline(stream, line);
    std::istringstream linestream(line);
    // starttime is the 22nd value, consume the stream to this value
    for (int i = 0; i < 21; i++) {
      linestream >> tmp;
    }
    linestream >> uptime;
  }
  return uptime;
}