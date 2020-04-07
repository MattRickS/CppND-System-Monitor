#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

float Processor::Utilization() {
  // Signature modified for more accurate reporting of values
  std::vector<float> values = LinuxParser::CpuUtilization();

  // CPU utilization determined using algorithm defined here:
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux

  int prev_idle = idle_;
  int prev_total = total_;

  // 0    1    2      3    4      5   6       7     8     9
  // user nice system idle iowait irq softirq steal guest guest_nice
  idle_ = values[3] + values[4];
  int nonidle =
      values[0] + values[1] + values[2] + values[5] + values[6] + values[7];
  total_ = idle_ + nonidle;

  // Define as floats to get accurate percentage
  float total_diff = total_ - prev_total;
  float idle_diff = idle_ - prev_idle;
  return (total_diff - idle_diff) / total_diff;
}