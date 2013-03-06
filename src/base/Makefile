# in order to generate base library
# By ZhouChao(zhouchao@ict.ac.cn)

AR = ar
ARFLAGS = cr
LIB = libbase.a
RM = rm -rf
OBJS = logging.o vcd_dir.o

ROOT_DIR=..

INCLUDE = -I$(ROOT_DIR) -I$(ROOT_DIR)/third_party

CC = g++
CFLAGS = -fPIC

SRCS = $(subst .o,.cc, $(OBJS))

UNITTEST = unittest_vcddir
LDFLAGS_TEST = -L../lib -lgtest -lpthread

all: $(LIB) $(UNITTEST)

#$(OBJS):$(SRCS)
#	$(CC) $(CFLAGS) -c $^ $(INCLUDE)

%.o: %.cc
	$(CC) $(CFLAGS) -c $< $(INCLUDE)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

unittest_vcddir: unittest_vcddir.o
	$(CC) $(CFLAGS) -o $@ $^ $(LIB) $(LDFLAGS_TEST)

clean:
	$(RM) $(OBJS) $(LIB) $(UNITTEST) *.o

.PHONY: all clean
