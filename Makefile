CC = g++
libCC = libtool --mode=link g++
ifeq ($(shell sw_vers 2>/dev/null | grep Mac | awk '{ print $$2}'),Mac)
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -I/usr/X11/include -DOSX  -Idevil-1.7.8/include -Ldevil-1.7.8/lib
	LDFLAGS = -framework GLUT -framework OpenGL \
    	-L"/System/Library/Frameworks/OpenGL.framework/Libraries" \
    	-lGL -lGLU -lm -lstdc++ -Ldevil-1.7.8/lib -Ldevil-1.7.8/src-IL
else
	CFLAGS = -g -DGL_GLEXT_PROTOTYPES -I./include/ -Iglut-3.7.6-bin -Idevil-1.7.8/include
	LDFLAGS = -lglut -lGL -lGLU -lm -lstdc++ -Ldevil-1.7.8/src-IL -Ldevil-1.7.8/lib devil-1.7.8/lib/libIL.la
endif
	
RM = /bin/rm -f 
all: main 
main: example_01.o 
	$(libCC) $(CFLAGS) -o as1 example_01.o $(LDFLAGS)
example_01.o: example_01.cpp
	$(CC) $(CFLAGS) -c example_01.cpp -o example_01.o 
clean: 
	$(RM) *.o as1
 


