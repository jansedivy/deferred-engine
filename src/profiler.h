#pragma once

#include <stdio.h>
#include <stdint.h>
#include <vector>

class Profiler {
  public:
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
    int depth = 0;

    std::vector<ProfileEvent> events;
};

#define PROFILE(name) global_profiler->start(name)
#define PROFILE_END() global_profiler->end()
#define PROFILER_EXPORT() global_profiler->exportData()

extern Profiler *global_profiler;
