# in order to generate base library
# By ZhouChao(zhouchao@ict.ac.cn)

AR = ar
ARFLAGS = cr
LIB = libfeature.a
RM = rm -rf
SRCS = color_hist.cc feature.cc img_saliency.cc om.cc \
	   segment_image.cc common.cc om_feature.cc \
	   common_index.cc
HEADS = *.h
OBJS = $(subst .cc,.o, $(SRCS))

CC = g++
ROOT_DIR=..

INCLUDE = -I$(ROOT_DIR)/third_party \
		  -I$(ROOT_DIR)
ifeq ($(OPENCV), ONLINE)
	INCLUDE += -I$(ROOT_DIR)/third_party/opencv
else
	INCLUDE += `pkg-config --cflags opencv`
endif

CFLAGS += -fPIC -Wall
ifeq ($(OPENCV), ONLINE)
	CFLAGS += -O2
else
	CFLAGS += -g
	CFLAGS += -DDEBUG
endif
CFLAGS += $(INCLUDE)

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

LDFLAGS_TEST = -L$(ROOT_DIR)/lib -lgtest -L$(ROOT_DIR)/base -lbase \
			   `pkg-config --libs opencv`

unittest_om: unittest_om.o $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB) $(LDFLAGS_TEST)

clean:
	$(RM) $(OBJS) $(LIB) *.o *.d *.a

.PHONY: all clean
