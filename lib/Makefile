
all: x-all

%.o:
	cd .. && make mservclient/$@ $(MAKEFLAGS)

%:
	cd .. && make $@ $(MAKEFLAGS)
