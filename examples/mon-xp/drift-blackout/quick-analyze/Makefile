TARGETS = drift-analyze simtime simtime-diff simtime-diff-seq simtime-diff-occ simtime-filter-gt

SRC  = $(wildcard *.c)
OBJS = $(SRC:.c=.o)
DEPS = $(SRC:.c=.d)

CFLAGS := -O3 -I/usr/local/include -fomit-frame-pointer -std=c99 \
	-pedantic -Wall -Wextra -MMD -pipe
LDFLAGS := -lgawen

ifdef VERBOSE
	Q :=
else
	Q := @
endif

.PHONY: all clean

all: $(TARGETS)

%.o: %.c
	@echo "===> CC $<"
	$(Q)$(CC) -c $(CFLAGS) -o $@ $<

drift-analyze: drift-analyze.o
	@echo "===> LD $@"
	$(Q)$(CC) $^ $(LDFLAGS) -o $@

simtime: simtime.o
	@echo "===> LD $@"
	$(Q)$(CC) $^ $(LDFLAGS) -o $@

simtime-diff: simtime-diff.o
	@echo "===> LD $@"
	$(Q)$(CC) $^ $(LDFLAGS) -o $@

simtime-diff-occ: simtime-diff-occ.o
	@echo "===> LD $@"
	$(Q)$(CC) $^ $(LDFLAGS) -o $@

simtime-diff-seq: simtime-diff-seq.o
	@echo "===> LD $@"
	$(Q)$(CC) $^ $(LDFLAGS) -o $@

simtime-filter-gt: simtime-filter-gt.o
	@echo "===> LD $@"
	$(Q)$(CC) $^ $(LDFLAGS) -o $@

clean:
	@echo "===> CLEAN"
	$(Q)rm -f *.o
	$(Q)rm -f *.d
	$(Q)rm -f $(TARGETS)

-include $(DEPS)
