#
# "make all" to build necessary executables.
#



LIBS=		`mysql_config --libs_r` -lrt

INC=		-I. `mysql_config --include`

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
	$(CC) load.o support.o mytnt.o $(LIBS) -o ../tpcc_load -ltarantool -lmsgpackc

../tpcc_start : $(OBJS)
	$(CC) $(OBJS) $(LIBS) -g -o ../tpcc_start -ltarantool -lmsgpackc

clean :
	rm -f *.o
