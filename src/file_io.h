#pragma once

#include <iostream>
#include <fstream>

#include <sys/stat.h>

namespace FileIO {
  std::string loadFile(const char *path);

  time_t getLastWriteTime(const char* filename);
}
