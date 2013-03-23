#METHOD = 2
#DEBUG = y

#
# opencv flags
# 1 for current path static library
# 2 for system dynamic library
#
#OPENCV = OFFLINE
#OPENCV = ONLINE

CC = g++
MAKE = make
LIB_NAME = libom.so
ROOT_DIR = .

################################################
#
# all targest
#

# exec
EXEC := run_test_om	run_test_sa
# unittest targets
UNITTEST = unittest_imagebuf unittest_ominterface
# all tests
TESTS = $(EXEC) $(UNITTEST)

# sub dir
SUB_MAKE = sub_make

ALL = $(SUB_MAKE) $(LIB_NAME)
ifneq ($(OPENCV), ONLINE)
	ALL += $(EXEC)
endif

ifeq ($(TEST), ON)
	ALL += $(UNITTEST)
endif

all: $(ALL)

################################################
#
# next build the libom.so
#

#
# cflags for build
#
CFLAGS = -fPIC 
# whether debug
ifeq ($(OPENCV), ONLINE)
	CFLAGS += -O2
else
	CFLAGS += -g
	CFLAGS += -DDEBUG
endif
CFLAGS += -Wall

ifeq ($(OPENCV), ONLINE)
	CFLAGS += -I$(ROOT_DIR)/third_party/opencv
else
	CFLAGS += `pkg-config --cflags opencv`
endif

#
# linker
#
LDFLAGS = -lpthread
ifeq ($(OPENCV), ONLINE)
	# opencv libs
	LDFLAGS += -L$(ROOT_DIR)/lib -lopencv_features2d -lopencv_imgproc \
			   -lopencv_core \
			   -lopencv_highgui
	# fro zlib
	LDFLAGS += -lz
	# for time lib
	LDFLAGS += -lrt
else 
	LDFLAGS += `pkg-config --libs opencv`
endif

# sub dir libs
LIBS := base/libbase.a feature/libfeature.a

#
# src files
#
SRCS = feature_db.cc \
	   frame.cc frame_index.cc om_interface.cc \
	   thread_safe_index.cc vcd_file.cc \
	   image_buf.cc
	   #imitation.cc utils.cc

# get objs from srcs
OBJS = $(subst .cc,.o,$(SRCS))

$(LIB_NAME): $(OBJS) $(LIBS)
	$(CC) -shared -o $@ $(OBJS) $(LIBS) $(LDFLAGS)

################################################
#
# first make the subdir to get the libs
#
#
SUBDIRS := base feature
.PHONY: sub_make
sub_make:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir OPECV=$(OPECV) all || exit 1;\
	done
# sub dirs


###############################################
#
# build Tests
#
#

FFMPEG_LINK_FLAGS = -L/usr/local/lib -lavcodec -lavformat -lavutil -lswscale

EXEC_LDFLAS = -L. -lom -Wl,-rpath=./:/usr/local/lib
#EXEC_LDFLAS += `pkg-config --libs opencv`
EXEC_LDFLAS += -L/usr/local/lib -lopencv_features2d -lopencv_imgproc \
		   -lopencv_core \
		   -lopencv_highgui
EXEC_LDFLAS += base/libbase.a feature/libfeature.a
EXEC_LDFLAS += $(FFMPEG_LINK_FLAGS)

EXEC_SRCS = imitation.cc utils.cc video_processor.cc
EXEC_OBJS = $(subst .cc,.o,$(EXEC_SRCS))

#
# exec depends like this:
#
# run_test_om: test_om.o
#run_test_om: test_om.o $(EXEC_OBJS)
#	@echo LINK Exec [$@]
#	$(CC) -o $@ $^ $(EXEC_LDFLAS)
#	@echo

run_%: %.o $(EXEC_OBJS)
	@echo LINK Exec [$@]
	$(CC) -o $@ $^ $(EXEC_LDFLAS)
	@echo

# unittest linker
LDFLAGS_TEST = lib/libgtest.a
LDFLAGS_TEST += $(FFMPEG_LINK_FLAGS)

# unittest cflags
CFLAGS_TEST = -I./third_party/

#
# generate unittest of imagebuf
# 
UNITTEST_IMAGEBUF_SRCS = image_buf.cc imitation.cc utils.cc		\
						 unittest_imagebuf.cc video_processor.cc
UNITTEST_IMAGEBUF_OBJS = $(subst .cc,.o,$(UNITTEST_IMAGEBUF_SRCS))
unittest_imagebuf: $(UNITTEST_IMAGEBUF_OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS) $(LDFLAGS_TEST)

#
# generate unittest of imagebuf
# 
UNITTEST_OMINTERFACE_SRCS = feature_db.cc \
						   	frame.cc frame_index.cc om_interface.cc \
						    thread_safe_index.cc utils.cc vcd_file.cc \
						    imitation.cc image_buf.cc \
							video_processor.cc unittest_ominterface.cc
UNITTEST_OMINTERFACE_OBJS = $(subst .cc,.o,$(UNITTEST_OMINTERFACE_SRCS))
unittest_ominterface: $(UNITTEST_OMINTERFACE_OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS) $(LDFLAGS_TEST)


################################################
#
# rules for build .cc files
#

#
# special rules for unittest_*.cc
# 
unittest_%.o: unittest_%.cc
	$(CC) $(CFLAGS) $(CFLAGS_TEST) -c $<

#
# rules for build .cc
#
%.o: %.cc
	$(CC) $(CFLAGS) -c $< 



################################################
#
# clean make
#
RM = rm -rf
clean:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean || exit 1; done
	$(RM) $(OBJS) $(EXEC) $(UNITTEST) *.o $(LIB_NAME)

.PHONY: all clean 
