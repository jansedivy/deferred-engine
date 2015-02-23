#include "error.h"

#include <SDL.h>

void showError(const char* title, const char* format, ...) {
  static const int size = 512;
  char message[size];

  va_list args;
  va_start(args, format);
  formatString(message, size, format, args);
  va_end(args);

  SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, NULL);
}
