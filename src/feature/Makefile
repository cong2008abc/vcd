# in order to generate base library
# By ZhouChao(zhouchao@ict.ac.cn)

AR = ar
ARFLAGS = cr
LIB = libfeature.a
RM = rm -rf
SRCS = color_hist.cc feature.cc img_saliency.cc om.cc \
	   segment_image.cc common.cc

HEADS = *.h
OBJS = $(subst .cc,.o, $(SRCS))

ROOT_DIR=..
INCLUDE = -I$(ROOT_DIR) -I$(ROOT_DIR)/third_party \
		  -I$(ROOT_DIR)/third_party/opencv

CC = g++
CFLAGS = -fPIC
CFLAGS += $(INCLUDE)
CFLAGS += -g
CFLAGS += -Wall
LDFLAGS = -lpthread
LDFLAGS_TEST = -L$(ROOT_DIR)/lib -lgtest -L$(ROOT_DIR)/base -lbase \
			   `pkg-config --libs opencv`

#UNITTEST = unittest_om

all: $(LIB) $(UNITTEST)

$(LIB):$(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#include $(SRCS:.cc=.d)

%.o: %.cc
	$(CC) $(CFLAGS) -c $< 

%.d: %.cc
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $(INCLUDE) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

unittest_om: unittest_om.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB) $(LDFLAGS_TEST)

clean:
	$(RM) $(OBJS) $(LIB) *.o *.d

.PHONY: all clean
