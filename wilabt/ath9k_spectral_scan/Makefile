
CFLAGS = -g -Wall
LIBS = -lm -lnl-genl-3 -lnl-3 -loml2 -locomm
INCLUDES = -I/usr/include/libnl3

all: ath_spec_scan

ath_spec_scan: ath_spec_scan.c
	rm -f ath_spec_scan.h;
	oml2-scaffold --oml ath_spec_scan.rb;
	gcc $(CFLAGS) -o ath_spec_scan ath_spec_scan.c $(INCLUDES) $(LIBS)

clean:
	rm -f ath_spec_scan ath_spec_scan_oml.h

