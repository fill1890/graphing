# Use Clang if it's available, otherwise use the default /usr/bin/cc
# Should be present on POSIX-compliant (and likely a number of non-POSIX
# compliant) platforms
CC:=$(shell command -v clang 2>/dev/null)
CC?=cc

# Use bash for script commands (Should be present on POSIX shells)
SHELL:=bash

# Pretty output (yes|no)
PRETTY?=yes
# Get colors for pretty output
include .colors.mk

# If make is run with no arguments, target all
# For output
MAKECMDGOALS?=all

# GCC doesn't have the -Weverything flag, so we just use the standards
# gnu-zero-variadic-macro-arguments is disabled because it breaks the
# debugging macros
ifneq (,$(findstring clang,$(CC)))
	W=-Weverything -Wno-gnu-zero-variadic-macro-arguments
else
	W=-Wall -Wextra
endif

# No problems whatsoever are allowed
CFLAGS=-g $(O) $(W) -Werror -Isrc -DLIB -DNDEBUG $(OPTFLAGS)
LIBS=-ldl -lm $(OPTLIBS)
PREFIX?=/usr/local

# ALl the .c files from the src/ directory
SOURCES:=$(wildcard src/**/*.c src/*.c)
OBJECTS:=$(patsubst %.c,%.o,$(SOURCES))

# All the _test.c files from the tests/ directory
TEST_SRC:=$(wildcard tests/*_tests.c)
TESTS:=$(patsubst %.c,%,$(TEST_SRC))

# Small programs from the bin/ directory
PROGRAMS_SRC:=$(wildcard bin/*.c)
PROGRAMS:=$(patsubst %.c,%,$(PROGRAMS_SRC))

# .a and .so targets
TARGET=build/libLIBRARY.a
SO_TARGET:=$(patsubst %.a,%.so,$(TARGET))

# Standard build: build .a library, .so library, build and run tests,
# and compile programs. Disables debug macros and runs optimizations.
all: pre-build $(TARGET) $(SO_TARGET) test $(PROGRAMS) post-build

# Development build: Same as standard, but allow debug macros, disable
# optimizations and allow warnings
# Disabling pretty output doesn't work, use PRETTY=no in shell
dev: CFLAGS=-g $(W) -Isrc $(OPTFLAGS)
dev: PRETTY:=no
dev: all

# Library targets
$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
ifeq ($(PRETTY),no)
	ar rcs $@ $(OBJECTS)
	ranlib $@
else
	@echo -e "\e[0;32mBuilding library \e[0;0m\e[0;33m$@\e[0;0m\e[0;32m...\e[0;0m"
	@ar rcs $@ $(OBJECTS)
	@ranlib $@
	@echo -e "\e[0;32mDone\e[0;0m"
endif

$(SO_TARGET): $(TARGET) $(OBJECTS)
ifeq ($(PRETTY),no)
	$(CC) -shared -o $@ $(OBJECTS)
else
	@echo -e "\e[0;32mBuilding library \e[0;0m\e[0;33m$@\e[0;0m\e[0;32m...\e[0;0m"
	@$(CC) -shared -o $@ $(OBJECTS)
	@echo -e "\e[0;32mDone\e[0;0m"
endif

# Build dependency - ensure build/ and bin/ are present
build:
	@mkdir -p build
	@mkdir -p bin

# Link against built library for bin/ programs
$(PROGRAMS): LDLIBS += $(TARGET)

# Pretty output for source targets
src/%.o: src/%.c
ifeq ($(PRETTY),no)
	$(CC) $(CFLAGS) $< $(LDLIBS) -c -o $@
else
	@echo -e "\e[0;32mCC \e[0;0m\e[0;34m$<\e[0;0m\e[0;32m -o \e[0;0m\e[0;33m$@\e[0;0m"
	@$(CC) $(CFLAGS) $< $(LDLIBS) -c -o $@
endif

# Pretty output for tests
tests/%: tests/%.c
ifeq ($(PRETTY),no)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@
else
	@echo -e "[TEST] \e[0;32mCC \e[0;0m\e[0;34m$<\e[0;0m\e[0;32m -o \e[0;0m\e[0;33m$@\e[0;0m"
	@$(CC) $(CFLAGS) $< $(LDLIBS) -o $@
endif

# Pretty output for programs
bin/%: bin/%.c
ifeq ($(PRETTY),no)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@
else
	@echo -e "[BIN] \e[0;32mCC \e[0;0m\e[0;34m$<\e[0;0m\e[0;32m -o \e[0;0m\e[0;33m$@\e[0;0m"
	@$(CC) $(CFLAGS) $< $(LDLIBS) -o $@
endif

# Link tests against built library and run
.PHONY: test
test: LDLIBS += $(TARGET)
test: $(TESTS)
	@$(SHELL) ./tests/runtests.sh

# Standard make, but run tests against valgrind
valgrind:
	VALGRIND="valgrind --quiet --log-file=/tmp/valgrind-%p.log" $(MAKE)

# Remove object files, test binaries, bin/ programs, test log, weird
# gcc files and weird dSYM directories
clean:
ifeq ($(PRETTY),no)
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f $(PROGRAMS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`
else
	@echo -e "\e[0;32mCleaning build\e[0;0m"
	@echo "Removing library, objects and tests..."
	@rm -rf build $(OBJECTS) $(TESTS)
	@echo "Removing binaries..."
	@rm -f $(PROGRAMS)
	@echo "Removing test logs..."
	@rm -rf tests/tests.log
	@echo "Removing build waste..."
	@find . -name "*.gc*" -exec rm {} \;
	@rm -rf `find . -name "*.dSYM" -print`
	@echo -e "\e[0;32mDone\e[0;0m"
endif

# Install to install directory, default /usr/local
install: all
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# Run a regex to find any bad string functions
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions:
	@egrep $(BADFUNCS) $(SOURCES) || true

# Notify build start
pre-build:
	@echo "[INFO] Building target $(MAKECMDGOALS) in $(notdir $(shell pwd))"

# Notify build completion
post-build:
	@echo "[INFO] Build completed"
