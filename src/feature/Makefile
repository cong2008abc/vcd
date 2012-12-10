cc = g++

FLAG = -fPIC

TEST = feature.o
TEST += om.o
TEST += color_hist.o

lib: $(TEST)
	$(cc) -shared -o libfeature.so $(FLAG) $(TEST)
feature.o: feature.cc
	$(cc) $(FLAG) -c $<
om.o: om.cc
	$(cc) $(FLAG) -c $<
color_hist.o: color_hist.cc
	$(cc) $(FLAG) -c $<

clean:
	rm $(TEST) libfeature.so
