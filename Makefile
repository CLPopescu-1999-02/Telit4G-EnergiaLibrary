CC = g++
OPTIMIZE = -O2
CFLAGS = -ggdb -w -std=c++11 $(OPTIMIZE)

default: unit LTEBase LTEHttp MockSerial
	$(CC) $(CFLAGS) -o unit_test unit.o LTEBase.o LTEHttp.o MockSerial.o

verbose:
	$(CC) -v $(CFLAGS) -o unit_test unit.o LTEBase.o LTEHttp.o

unit:
	$(CC) $(CFLAGS) -c unit_test.cpp -o unit.o

LTEBase:
	$(CC) $(CFLAGS) -c LTEBase.cpp -o LTEBase.o

LTEHttp:
	$(CC) $(CFLAGS) -c LTEHttp.cpp -o LTEHttp.o

MockSerial:
	$(CC) $(CFLAGS) -c MockSerial.cpp -o MockSerial.o

clean:
	rm -rf *.o *.dSYM unit_test
