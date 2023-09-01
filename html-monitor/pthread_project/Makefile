
CROSS_COMPILE:=

ifeq ($(ARCH),arm)
CROSS_COMPILE=arm-linux-gnueabihf-
else
CROSS_COMPILE=
endif
CC=$(CROSS_COMPILE)gcc
CFLAGS= -c -g
#LDFLAGS= -lpthread -L ./lib -lsqlite3
LDFLAGS= -lpthread

OBJS=main.o   \
	 pthread_function.o global.o

monitor_obj :$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
	
$(OBJS):%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

copy:
	sudo cp monitor_obj ~/workdir/

.PHONY:clean
clean:
	rm *.o monitor_obj -rf 

