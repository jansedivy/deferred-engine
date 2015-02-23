#include "file_io.h"

std::string FileIO::loadFile(const char *path) {
  std::string content;
  std::ifstream fileStream(path, std::ios::in);

  if (!fileStream.is_open()) {
    showError("Error loading file", "Could not read file %s. File does not exist."); // path
  }

  std::string line;

  while (!fileStream.eof()) {
    std::getline(fileStream, line);
    content.append(line + "\n");
  }

  fileStream.close();

  return content;
}

time_t FileIO::getLastWriteTime(const char* filename) {
  time_t LastWriteTime = 0;

  struct stat FileStat;
  if (stat(filename, &FileStat) == 0) {
    LastWriteTime = FileStat.st_mtimespec.tv_sec;
  }

  return LastWriteTime;
}
