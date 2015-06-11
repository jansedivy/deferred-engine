#include "profiler.h"

#include <SDL.h>

Profiler *global_profiler = 0;

void Profiler::start(const char *name) {
  if (events.size() < 1024) {
    ProfileEvent event;
    event.name = name;
    event.start = SDL_GetPerformanceCounter();
    event.depth = depth;

    events.push_back(event);
    stack[depth++] = events.size() - 1;
  }
}

void Profiler::end() {
  if (depth > 0) {
    ProfileEvent &event = events.at(stack[depth - 1]);
    event.end = SDL_GetPerformanceCounter();
    depth = depth - 1;
  }
}

void Profiler::exportData() {
  const int resolution = (int)(SDL_GetPerformanceFrequency() / 1000000);

  SDL_RWops *file = SDL_RWFromFile("../../../performance.log", "w");
  if (file) {
    for (auto it = events.begin(); it != events.end(); it++) {
      char buf[1024];

      int index = 0;

      for (int i=0; i<it->depth; i++) {
        if (index < 1024) {
          buf[index++] = ' ';
        }
      }

      int microseconds = (int)((it->end - it->start) / resolution);
      sprintf(buf+index, "%s: %d\n", it->name, microseconds);

      SDL_RWwrite(file, buf, 1, strlen(buf));
    }
  }
}
