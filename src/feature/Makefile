# in order to generate base library
# By ZhouChao(zhouchao@ict.ac.cn)

AR = ar
ARFLAGS = cr
LIB = libfeature.a
RM = rm -rf
SRCS = color_hist.cc feature.cc img_saliency.cc om.cc \
	   segment_image.cc

HEADS = *.h
OBJS = $(subst .cc,.o, $(SRCS))

ROOT_DIR=..
INCLUDE = -I$(ROOT_DIR) -I$(ROOT_DIR)/third_party

CC = g++
CFLAGS = -fPIC `pkg-config --cflags opencv`
CFLAGS += -g
CFLAGS += -Wall
LDFLAGS = -lpthread
LDFLAGS_TEST = -L$(ROOT_DIR)/lib -lgtest -L$(ROOT_DIR)/base -lbase \
			   `pkg-config --libs opencv`

UNITTEST = unittest_om

all: $(LIB) $(UNITTEST)

#$(AR) $(ARFLAGS) $(CFLAGS) $(LDFLAGS) $@ $(OBJS)
$(LIB):$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#$(OBJS):$(SRCS) $(HEADS)
#	$(CC) $(CFLAGS) -c $^ $(INCLUDE)

#include $(SRCS:.cc=.d)

%.o: %.cc
	$(CC) $(CFLAGS) -c $< $(INCLUDE)

unittest_om: unittest_om.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB) $(LDFLAGS_TEST)

clean:
	$(RM) $(OBJS) $(LIB) *.o

.PHONY: all clean
