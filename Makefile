# e9ebd225944b2630f4dd36def9a269f7
# Generated with vfnmkmc by the mc program.
PREFIX=/usr/local
CFLAGS=-O$(O)  -std=c++11
O=2
LFLAGS= -l jsoncpp
OBJS=objs/options.o objs/jconfig.o objs/commands.o objs/rndstream.o objs/strings.o objs/pcre_utils.o objs/utils.o objs/doubles.o objs/json_utils.o objs/rand_utils.o objs/system.o objs/vectors.o objs/radix.o


.PHONY: all
all: objs rndstream

./rndstream:  $(OBJS)
	@ echo "    LINK ./rndstream"
	@ $(CXX) $(OBJS) -o "./rndstream" $(LFLAGS)

objs/options.o: src/options.cpp src/options.hpp src/tools.hpp
	@ echo "    CXX  src/options.cpp"
	@ $(CXX) $(CFLAGS) -c "src/options.cpp" -o $@
objs/jconfig.o: src/jconfig.cpp src/jconfig.hpp src/tools.hpp
	@ echo "    CXX  src/jconfig.cpp"
	@ $(CXX) $(CFLAGS) -c "src/jconfig.cpp" -o $@
objs/commands.o: src/commands.cpp src/commands.hpp src/tools.hpp
	@ echo "    CXX  src/commands.cpp"
	@ $(CXX) $(CFLAGS) -c "src/commands.cpp" -o $@
objs/rndstream.o: src/rndstream.cpp src/tools.hpp src/jconfig.hpp \
 src/options.hpp src/commands.hpp src/sorters.hpp
	@ echo "    CXX  src/rndstream.cpp"
	@ $(CXX) $(CFLAGS) -c "src/rndstream.cpp" -o $@
objs/strings.o: src/tools/strings.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/strings.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/strings.cpp" -o $@
objs/pcre_utils.o: src/tools/pcre_utils.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/pcre_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/pcre_utils.cpp" -o $@
objs/utils.o: src/tools/utils.cpp src/tools/../sorters.hpp \
 src/tools/../tools.hpp
	@ echo "    CXX  src/tools/utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/utils.cpp" -o $@
objs/doubles.o: src/tools/doubles.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/doubles.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/doubles.cpp" -o $@
objs/json_utils.o: src/tools/json_utils.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/json_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/json_utils.cpp" -o $@
objs/rand_utils.o: src/tools/rand_utils.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/rand_utils.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/rand_utils.cpp" -o $@
objs/system.o: src/tools/system.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/system.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/system.cpp" -o $@
objs/vectors.o: src/tools/vectors.cpp src/tools/../tools.hpp
	@ echo "    CXX  src/tools/vectors.cpp"
	@ $(CXX) $(CFLAGS) -c "src/tools/vectors.cpp" -o $@
objs/radix.o: src/sorters/radix.cpp src/sorters/../sorters.hpp
	@ echo "    CXX  src/sorters/radix.cpp"
	@ $(CXX) $(CFLAGS) -c "src/sorters/radix.cpp" -o $@

objs:
	@ mkdir "objs"

.PHONY: c clean
c: clean
clean:
	@ if [ -d "objs" ]; then rm -r "objs"; fi
	@ rm -f "./rndstream"
	@ echo "    CLEAN"
.PHONY: f fresh
f: fresh
fresh: clean
	@ make all --no-print-directory
.PHONY: r run
r: run
run: all
	@ ././rndstream

.PHONY: d debug
d: debug
debug: CFLAGS += -DDEBUG -g3 -Wall -Wextra
debug: O=0
debug: all

.PHONY: sc superclean
sc: superclean
superclean: clean
	@ rm -rf "deps"
	@ echo "    SUPERCLEAN"

.PHONY: install
install: all
	@ install -D -m 755 rndstream $(PREFIX)/bin/rndstream
	@ echo "[1;32m*[0m rndstream installed in $(PREFIX)/bin"
	@ echo "[1muse \`make uninstall\` to remove rndstream[0m"

.PHONY: uninstall
uninstall:
	@ rm $(PREFIX)/bin/rndstream
	@ echo "[1;32m*[0m rndstream removed from $(PREFIX)/bin[0m"

.PHONY: check-syntax
check-syntax:
	$(CXX) $(CFLAGS) -fsyntax-only -Wall -o /dev/null -S src/*
