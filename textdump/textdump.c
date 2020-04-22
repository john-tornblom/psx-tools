/*
Copyright (c) 2020, John Törnblom
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>


static void
dump_string(const char* str, size_t maxlen) {
  size_t i = 0;

  fprintf(stdout, "\"");

  while(i < maxlen) {
    switch(str[i]) {
    case '\a':
      fprintf(stdout, "\\a");
      break;

    case '\b':
      fprintf(stdout, "\\b");
      break;

    case '\e':
      fprintf(stdout, "\\e");
      break;

    case '\f':
      fprintf(stdout, "\\f");
      break;

    case '\n':
      fprintf(stdout, "\\n");
      break;

    case '\r':
      fprintf(stdout, "\\r");
      break;

    case '\t':
      fprintf(stdout, "\\t");
      break;

    case '\v':
      fprintf(stdout, "\\v");
      break;

    case '\\':
      fprintf(stdout, "\\\\");
      break;

      /*
    case '\'':
      fprintf(stdout, "\\\'");
      break;

    case '\?':
      fprintf(stdout, "\\?");
      break;
      */

    case '\"':
      fprintf(stdout, "\\\"");
      break;

    case '\0':
      i = maxlen;
      break;

    default:
      fprintf(stdout, "%c", str[i]);
    }
    i++;
  }

  fprintf(stdout, "\"\n");
}


int
main(int argc, char *argv[]) {
  FILE *fp;
  size_t size, i;
  char* str;

  if(argc < 2) {
    fprintf(stderr, "Usage: textdump [file]\n");
    return 0;
  }

  if(!(fp = fopen(argv[1], "rb"))) {
    fprintf(stderr, "Unable to open %s: %s\n",
	    argv[1], strerror(errno));
    exit(1);
  }

  fseek(fp, 0, SEEK_END);
  size = ftell(fp);
  rewind(fp);

  str = malloc(size + 1);
  fread(str, 1, size, fp);
  fclose(fp);

  str[size] = '\0';

  i = 0;
  if(size < 4) {
    fprintf(stderr, "Unexpected end of file\n");
    exit(1);
  }

  // there is a binary preemble of the format XX YY 00 00,
  // Lets just skip that one
  while(i < size - 4 && !str[i+2] && !str[i+3]) {
    i += 4;
  }

  //Next follows null-terminated (iso-8859-14?) strings
  while(i < size) {
    if(str[i]) {
      dump_string(&str[i], size - i);
      i += strnlen(&str[i], size - i);
    } else {
      i++;
    }
  }

  free(str);

  return 0;
}