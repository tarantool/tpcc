#
# "make all" to build necessary executables.
#



LIBS=		-lrt -lpthread -lm -ltarantool -lmsgpackc

INC=		-I.

#DEFS=		-DDEBUG

CFLAGS=		-w -O3 -g

TRANSACTIONS=	neword.o payment.o ordstat.o delivery.o slev.o
OBJS=		main.o spt_proc.o driver.o support.o sequence.o rthist.o sb_percentile.o mytnt.o $(TRANSACTIONS)

.SUFFIXES:
.SUFFIXES: .o .c

.c.o:
	$(CC) $(CFLAGS) $(INC) $(DEFS) -c $*.c

all: ../tpcc_load ../tpcc_start

../tpcc_load : load.o support.o  mytnt.o
	$(CC) load.o support.o mytnt.o $(LIBS) -o ../tpcc_load

../tpcc_start : $(OBJS)
	$(CC) $(OBJS) $(LIBS) -g -o ../tpcc_start

clean :
	rm -f *.o
