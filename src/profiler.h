#pragma once

#include <stdio.h>
#include <stdint.h>
#include <vector>

class Profiler {
  public:
    Profiler();
    void start(const char *name);
    void end();
    void exportData();

  private:
    struct ProfileEvent {
      const char *name;
      uint64_t start;
      uint64_t end;
      int depth;
    };

    int stack[32];
    int depth;

    std::vector<ProfileEvent> events;
};
