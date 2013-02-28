# in order to generate base library
# By ZhouChao(zhouchao@ict.ac.cn)

AR = ar
ARFLAGS = cr
LIB = libbase.a
RM = rm -rf
OBJS = logging.o test_logging.o

ROOT_DIR=..

INCLUDE = -I$(ROOT_DIR) -I$(ROOT_DIR)/third_party

CC = g++
CFLAGS = -fPIC

SRCS = $(subst .o,.cc, $(OBJS))

all: $(LIB)

$(OBJS):$(SRCS)
	$(CC) $(CFLAGS) -c $^ $(INCLUDE)

$(LIB): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

clean:
	$(RM) $(OBJS) $(LIB)

.PHONY: all clean
