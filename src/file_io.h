#pragma once

#include <iostream>
#include <fstream>
#include "error.h"

#include <sys/stat.h>

namespace FileIO {
  std::string loadFile(const char *path);

  time_t getLastWriteTime(const char* filename);
}
