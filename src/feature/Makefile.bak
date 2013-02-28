cc = g++
AR = ar

FLAG = -fPIC
FLAG += `pkg-config --libs opencv --cflags opencv`

TEST = feature.o
TEST += om.o
TEST += color_hist.o
TEST += saliency.o
TEST += img_saliency.o

main: lib static

lib: $(TEST)
	$(cc) -shared -o libfeature.so $(FLAG) $(TEST)

static: $(TEST)
	$(AR) -cvr libfeature.a $(TEST)	

feature.o: feature.cc
	$(cc) $(FLAG) -c $<
om.o: om.cc
	$(cc) $(FLAG) -c $<
color_hist.o: color_hist.cc
	$(cc) $(FLAG) -c $<
saliency.o: saliency.cc
	$(cc) $(FLAG) -c $<
img_saliency.o: img_saliency.cc
	$(cc) $(FLAG) -c $<

clean:
	rm $(TEST) libfeature.so
