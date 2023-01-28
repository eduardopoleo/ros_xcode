//
//  file.c
//  ros_xcode
//
//  Created by Eduardo Poleo on 2023-01-23.
//

#include <stdio.h>
#include <stdlib.h>
#include "file.h"

char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(fileSize + 1);
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}
