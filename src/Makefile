cc = g++

#FLAG = -L. -lfeature -Wl,-rpath,./
FLAG = -fPIC -g
LFLAG = -L. libfeature.a
SFLAG = `pkg-config --libs opencv --cflags opencv`

TEST = info_db.o
TEST += data_stream.o
TEST += feature_db.o
TEST += utils.o
TEST += frame.o
TEST += frame_index.o
TEST += thread_safe_index.o
TEST += sa_cvpr_09.o
#TEST += om_interface.o

OM_LIB = frame.o
OM_LIB += frame_index.o
OM_LIB += om_interface.o
OM_LIB += vcd_file.o
OM_LIB += thread_safe_index.o

#for online test, only make for om lib!
all: om

detector:
	cd ./feature && make
	cp ./feature/libfeature.a ./.
	cp ./feature/libfeature.so ./.

build: $(TEST) build_lib.o
	$(cc) -o build_lib build_lib.o $(TEST) $(SFLAG) $(FLAG) $(LFLAG)

test_om: om test_om.o $(TEST) om
	$(cc) -o test_om test_om.o $(TEST) $(SFLAG) $(FLAG) -L. -lom -Wl,-rpath,./.

test_sa: test_sa.o $(TEST)
	$(cc) -o test_sa test_sa.o $(TEST) $(SFLAG) $(FLAG) -L. -lfeature -Wl,-rpath,./.

om: $(OM_LIB)
	$(cc) -shared -o libom.so $(OM_LIB) $(FLAG) $(LFLAG)

main.o: main.cc
	$(cc) $(FLAG) -c $< 
test_om.o: test_om.cc
	$(cc) $(FLAG) $(SFLAG) -c $< 
test_sa.o: test_sa.cc
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
thread_safe_index.o: thread_safe_index.cc
	$(cc) $(FLAG) -c $<
om_interface.o: om_interface.cc
	$(cc) $(FLAG) -c $<
vcd_file.o: vcd_file.cc
	$(cc) $(FLAG) -c $<
sa_cvpr_09.o: sa_cvpr_09.cc
	$(cc) $(FLAG) $(SFLAG) -c $< 

clean:
	rm $(TEST) $(OM_LIB) build_lib.o test_om.o build_lib test_om
