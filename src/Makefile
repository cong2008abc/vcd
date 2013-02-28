DEBUG = y

CC = g++
MAKE = make
CFLAGS = -fPIC `pkg-config --cflags opencv`
CFLAGS += -Wall
LDFLAGS = `pkg-config --libs opencv`

# whether debug
ifeq ($(DEBUG), y)
	CFLAGS += -g
else
	CFLAGS += -O2
endif

SUBDIRS := base feature
LIBS := base/libbase.a feature/libfeature.a lib/libgtest.a

LDFLAGS += $(LIBS)

#target = run_test_sa

SRCS = test_sa.cc feature_db.cc data_stream.cc \
	   frame.cc frame_index.cc info_db.cc om_interface.cc \
	   sa_cvpr_09.cc thread_safe_index.cc utils.cc vcd_file.cc
OBJS = $(subst .cc,.o,$(SRCS))

all: target

target: $(OBJS) 
	for dir in $(SUBDIRS);\
	do $(MAKE) -C $$dir all || exit 1;\
	done
	$(CC) -o run_test_sa $(CFLAGS) $(OBJS) $(LIBS) $(LDFLAGS)

%.o: %.cc
	$(CC) $(CFLAGS) -c $<

RM = rm -rf
clean:
	@for dir in $(SUBDIRS); do $(MAKE) -C $$dir clean || exit 1; done
	$(RM) $(OBJS) $(target)

.PHONY: all clean sublib
