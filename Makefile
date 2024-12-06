CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
 
9cc: $(OBJS)
				$(CC) -o 9cc $(OBJS) $(LDFLAGS)

$(OBJS): 9cc.h

callee:
				$(CC) callee.c -c

test: 9cc callee
				./test.sh

disasm:
				objdump tmp -d -M intel > tmp_disassemble.s

clean:
				rm -f 9cc *.o *~ tmp*

.PHONY: test clean
