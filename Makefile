CC = gcc
CFLAGS = -Wall -g -O2 
LDFLAGS = -lm

BINDIR = /usr/bin
DATADIR = /usr/share/strokeorder/kanji

strokeorder: main.c
	$(CC) $(CFLAGS) main.c -o strokeorder $(LDFLAGS)


install: strokeorder
	@echo "Installing binary to $(BINDIR)..."
	install -Dm755 strokeorder $(BINDIR)/strokeorder
	@echo "Installing data to $(DATADIR)..."
	install -d $(DATADIR)
	cp -r kanji/* $(DATADIR)/

uninstall:
	rm -f $(BINDIR)/strokeorder
	rm -rf /usr/share/strokeorder

clean:
	rm -f strokeorder
