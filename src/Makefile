METHOD = 2
DEBUG = y

CC = g++
MAKE = make
CFLAGS = -fPIC `pkg-config --cflags opencv`
CFLAGS += -Wall
LDFLAGS = `pkg-config --libs opencv` -lpthread

# whether debug
ifeq ($(DEBUG), y)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

SUBDIRS := base feature
LIBS := base/libbase.a feature/libfeature.a
#LIBS +=lib/libgtest.a

INCLUDE = -I./third_party
#target = run_test_sa

SRCS = feature_db.cc \
	   frame.cc frame_index.cc om_interface.cc \
	   thread_safe_index.cc utils.cc vcd_file.cc \
	   imitation.cc image_buf.cc

ifeq ($(METHOD), 1)
# test the whole algorithm
SRCS += test_om.cc
RUN_NAME = run_test_om
else
# test the saliency 
SRCS += test_sa.cc
RUN_NAME = run_test_sa
endif

OBJS = $(subst .cc,.o,$(SRCS))

# unittest target
UNITTEST = unittest_imagebuf unittest_ominterface
LDFLAGS_TEST = -L./lib -lgtest
#INCLUDE_TEST = -I./third_party

all: target $(UNITTEST) 

#
# main run app to test the while program
#
target: $(OBJS) 
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir all || exit 1;\
	done
	$(CC) -o run_test $(CFLAGS) $(OBJS) $(LIBS) $(LDFLAGS)
	cp run_test $(RUN_NAME)

#
# rules for build .cc
#
%.o: %.cc
	$(CC) $(CFLAGS) -c $< $(INCLUDE)

#
# generate unittest of imagebuf
# 
UNITTEST_IMAGEBUF_SRCS = image_buf.cc imitation.cc utils.cc		\
						 unittest_imagebuf.cc
UNITTEST_IMAGEBUF_OBJS = $(subst .cc,.o,$(UNITTEST_IMAGEBUF_SRCS))
unittest_imagebuf: $(UNITTEST_IMAGEBUF_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS_TEST) $(LDFLAGS)

#
# generate unittest of imagebuf
# 
UNITTEST_OMINTERFACE_SRCS = feature_db.cc \
						   	frame.cc frame_index.cc om_interface.cc \
						    thread_safe_index.cc utils.cc vcd_file.cc \
						    imitation.cc image_buf.cc unittest_ominterface.cc
UNITTEST_OMINTERFACE_OBJS = $(subst .cc,.o,$(UNITTEST_OMINTERFACE_SRCS))
unittest_ominterface: $(UNITTEST_OMINTERFACE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) $(LDFLAGS_TEST) $(LDFLAGS)

#
# clean make
#
RM = rm -rf
clean:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean || exit 1; done
	$(RM) $(OBJS) $(target) run_test* $(UNITTEST)

.PHONY: all clean sublib
