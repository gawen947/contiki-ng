/* Copyright (c) 2019, David Hauweele <david@hauweele.net>
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
   ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Report the difference between consecutive simulation times in the trace. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <err.h>

#include <gawen/iobuf.h>
#include <gawen/string.h>
#include <gawen/xatoi.h>

#define print(s) iobuf_write(out, s, strlen(s))

#ifdef __linux__
/* memmem in Linux requires _GNU_SOURCE
   but it also declares basename() with
   a GNU specific prototype which conflict
   with libgawen. */
void *memmem(const void *haystack, size_t haystacklen,
             const void *needle, size_t needlelen);
#endif


#define BUF_SIZE (4096)

#define SIM_FIELD "SIM_TIME_US"

int main(void)
{
  uint64_t simtime, old_simtime = (uint64_t)-1;
  iofile_t in, out;
  char *s, *t;
  int n;

  in  = iobuf_dopen(STDIN_FILENO);
  out = iobuf_dopen(STDOUT_FILENO);
  if(!out || !in)
    err(EXIT_FAILURE, "iobuf_dopen()");

  while(1) {
    /* we use the same buffer for reading
       and convert the diff time to int */
    char buf[BUF_SIZE];

    n = iobuf_gets(in, buf, BUF_SIZE);
    if(n <= 0) {
      if(n < 0)
        err(EXIT_FAILURE, "read()");
      else
        break;
    } else {
      if(buf[n - 1] == '\n')
        buf[--n] = '\0';
    }

    /* In the code below, we don't check for null pointers,
       it will segfault but we don't care, we assume
       that data is correctly formated if the check above
       is verified. */

    /* sim field */
    s  = memmem(buf, n, SIM_FIELD, strlen(SIM_FIELD));
    s += strlen(SIM_FIELD) + 1;
    t  = s;
    while(isdigit(*s++));
    *(s-1) = '\0';

    /* parse sim time as an integer */
    simtime = xatou64(t, &n);
    if(n)
      return 1;

    if(old_simtime != ((uint64_t)-1)) {
      n = sprintf(buf, "%lu\n", simtime - old_simtime);
      iobuf_write(out, buf, n);
    }

    old_simtime = simtime;
  }

  iobuf_close(out);

  return 0;
}
