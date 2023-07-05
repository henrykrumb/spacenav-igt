obj = src/spnavigt.o
bin = spnavigt

CC = g++
CFLAGS = -pedantic -Wall -Wextra -O3 -I/usr/local/include/igtl
LDFLAGS = /usr/local/lib/libspnav.a -L/usr/local/lib/igtl -lX11 -lOpenIGTLink

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) $(bin)
