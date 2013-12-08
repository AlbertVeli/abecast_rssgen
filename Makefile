DIR = target
FEED = $(DIR)/feed.rss
GENDIR = abecast_rssgen
GEN = $(GENDIR)/abecast_rssgen

# Define RSYNC and URL in make.inc. It could look like:
# RSYNC = rsync -vaz
# URL = example.com:/var/www/podcast/
include make.inc

all: $(FEED)

$(FEED): $(GEN) header.html.template
	./generate.sh $(DIR)
	$(GEN) $(DIR) > $@

$(GEN): $(GEN).o
	make -C $(GENDIR)

.PHONY: sync clean

sync: $(FEED)
	$(RSYNC) $(DIR)/ $(URL)

clean:
	make -C $(GENDIR) clean
	rm -f *~ $(TARGET)/*~ $(FEED) header.html
