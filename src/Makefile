#METHOD = 2
#DEBUG = y

#
# opencv flags
# 1 for current path static library
# 2 for system dynamic library
#
OPENCV = 1

CC = g++
MAKE = make
LIB_NAME = libom.so

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

all: $(SUB_MAKE) $(LIB_NAME) 

################################################
#
# first make the subdir to get the libs
#
#
.PHONY: sub_make
sub_make:
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir all || exit 1;\
	done
# sub dirs
SUBDIRS := base feature


################################################
#
# next build the libom.so
#

#
# cflags for build
#
CFLAGS = -fPIC 
# whether debug
ifeq ($(DEBUG), y)
	CFLAGS += -g
else
	CFLAGS += -O2
endif
CFLAGS += -Wall

ifeq ($(OPENCV), 1)
	CFLAGS += -I./third_party/opencv
else
	CFLAGS += `pkg-config --cflags opencv`
endif

#
# linker
#
LDFLAGS = -lpthread
ifeq ($(OPENCV), 1)
	# opencv libs
	LDFLAGS += -L./lib -lopencv_features2d -lopencv_imgproc \
			   -lopencv_core \
			   -lopencv_highgui -lopencv_ml \
			   -lopencv_video -lopencv_calib3d \
			   -lopencv_objdetect -lopencv_contrib \
			   -lopencv_legacy -lopencv_flann
	# opencv 3rdparty libs
	LDFLAGS += -lIlmImf -ljpeg -lpng -ltiff -lzlib -ljasper
	# for time lib
	LDFLAGS += -lrt
	# for gtk
	#LDFLAGS += `pkg-config --libs gtk+-2.0`
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
	   image_buf.cc \
	   imitation.cc utils.cc

# get objs from srcs
OBJS = $(subst .cc,.o,$(SRCS))

$(LIB_NAME): $(OBJS) $(LIBS)
	$(CC) -shared -o $@ $(OBJS) $(LIBS) $(LDFLAGS)


###############################################
#
# build Tests
#
#


EXEC_LDFLAS = -L. -lom -Wl,-rpath=./ 
EXEC_SRCS = imitation.cc utils.cc
#EXEC_OBJS = $(subst .cc,.o,$(EXEC_SRCS))

#
# exec depends like this:
#
# run_test_om: test_om.o
run_%: %.o $(EXEC_OBJS)
	@echo LINK Exec [$@]
	$(CC) -o $@ $^ $(EXEC_LDFLAS)
	@echo


# unittest linker
LDFLAGS_TEST = -L./lib -lgtest

# unittest cflags
CFLAGS_TEST = -I./third_party/

#
# generate unittest of imagebuf
# 
UNITTEST_IMAGEBUF_SRCS = image_buf.cc imitation.cc utils.cc		\
						 unittest_imagebuf.cc
UNITTEST_IMAGEBUF_OBJS = $(subst .cc,.o,$(UNITTEST_IMAGEBUF_SRCS))
unittest_imagebuf: $(UNITTEST_IMAGEBUF_OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS_TEST) $(LDFLAGS)

#
# generate unittest of imagebuf
# 
UNITTEST_OMINTERFACE_SRCS = feature_db.cc \
						   	frame.cc frame_index.cc om_interface.cc \
						    thread_safe_index.cc utils.cc vcd_file.cc \
						    imitation.cc image_buf.cc unittest_ominterface.cc
UNITTEST_OMINTERFACE_OBJS = $(subst .cc,.o,$(UNITTEST_OMINTERFACE_SRCS))
unittest_ominterface: $(UNITTEST_OMINTERFACE_OBJS)
	$(CC) -o $@ $^ $(LIBS) $(LDFLAGS_TEST) $(LDFLAGS)


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
