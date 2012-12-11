cc = g++

FLAG = -L. -lfeature -Wl,-rpath,./
SFLAG = `pkg-config --libs opencv --cflags opencv`

TEST = info_db.o
TEST += data_stream.o
TEST += feature_db.o
TEST += utils.o
TEST += frame.o
TEST += frame_index.o
TEST += om_interface.o

all: detector test_om

detector:
	cd ./feature && make
	cp ./feature/libfeature.so ./.

build: $(TEST) build_lib.o
	$(cc) -o build_lib build_lib.o $(TEST) $(SFLAG) $(FLAG) 

test_om: $(TEST) test_om.o
	$(cc) -o test_om test_om.o $(TEST) $(SFLAG) $(FLAG)

main.o: main.cc
	$(cc) $(FLAG) -c $< 
test_om.o: test_om.cc
	$(cc) $(FLAG) $(SFLAG) -c $< 
build_lib.o: build_lib.cc
	$(cc) $(FLAG) -c $< 
info_db.o: info_db.cc
	$(cc) $(FLAG) -c $<
data_stream.o: data_stream.cc
	$(cc) $(FLAG) $(SFLAG) -c $<
feature_db.o: feature_db.cc
	$(cc) $(FLAG) -c $<
utils.o: utils.cc
	$(cc) $(FLAG) $(SFLAG) -c $<
frame.o: frame.cc
	$(cc) $(FLAG) -c $<
frame_index.o: frame_index.cc
	$(cc) $(FLAG) -c $<
om_interface.o: om_interface.cc
	$(cc) $(FLAG) -c $<

clean:
	rm $(TEST) build_lib test
