#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#  Copyright (c) 2019, David Hauweele <david@hauweele.net>
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#   1. Redistributions of source code must retain the above copyright notice, this
#      list of conditions and the following disclaimer.
#   2. Redistributions in binary form must reproduce the above copyright notice,
#      this list of conditions and the following disclaimer in the documentation
#      and/or other materials provided with the distribution.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import sys

if len(sys.argv) != 2:
    print("usage: %s BIN-WIDTH" % (sys.argv[0],))
    print("")
    print("Read value from stdin and count the number of occurence")
    print("for each values within the special interval.")
    sys.exit(1)

try:
    bin_width = float(sys.argv[1])
except:
    print("error: cannot parse '%s', should be the bin width" % (sys.argv[1],))
    sys.exit(1)

occ = {}
for line in sys.stdin:
    if line == "" or line[0] == '#':
        continue

    try:
        value = float(line)
    except:
        print("error: cannot parse '%s'" % (line,))
        sys.exit(1)

    value = int(value / bin_width) * bin_width

    if value not in occ:
        occ[value]  = 1
    else:
        occ[value] += 1

sorted_values = occ.keys()
sorted_values.sort()

print("# value #occ")
for value in sorted_values:
    print("%f %d" % (value, occ[value]))

