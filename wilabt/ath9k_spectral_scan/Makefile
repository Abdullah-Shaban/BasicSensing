
CFLAGS = -g -Wall
LIBS = -lm -lnl-genl-3 -lnl-3
INCLUDES = -I/usr/include/libnl3
MYSQL_INCLUDE = `mysql_config --cflags --libs`

all: ath_spec_scan

ath_spec_scan: ath_spec_scan.c
	gcc $(CFLAGS) -o ath_spec_scan ath_spec_scan.c $(INCLUDES) $(LIBS) $(MYSQL_INCLUDE)

clean:
	rm -f ath_spec_scan
