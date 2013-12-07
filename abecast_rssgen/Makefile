eXe = abecast_rssgen
OBJS = $(eXe).o

DEBUG = yes
ifdef DEBUG
DBGFLAGS = -g -O0
else
DBGFLAGS = -O3
endif
CFLAGS = $(DBGFLAGS) -pipe -W -Wall -std=gnu99 -pedantic
LIBS = -lid3tag -lz

all: $(eXe)

$(eXe): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

.PHONY: clean check

check:
	clang --analyze $(CFLAGS) $(OBJS:.o=.c)

clean:
	rm -f core $(eXe) $(OBJS) *.plist *~
