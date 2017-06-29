#
# From http://www.borngeek.com/2010/05/06/automatic-dependency-generation/
#

SHELL = /bin/bash
TARGET = main
 
ifndef BC
    BC=debug
endif
 
CC = g++
CFLAGS = -Wall -Wextra -std=c++11 -DSINGLE_MODE
LDFLAGS = -lm -lrt -lpthread
 
ifeq ($(BC),debug)
CFLAGS += -g
else
CFLAGS += -O2
endif
 
DEPDIR=deps
OBJDIR=$(BC)/objs
SRCDIR=src
 
OTMP = $(patsubst $(SRCDIR)/%.cpp,%.o,$(wildcard $(SRCDIR)/*.cpp))
OBJS = $(patsubst %,$(OBJDIR)/%,$(OTMP))
DEPS = $(patsubst %.o,$(DEPDIR)/%.d,$(OTMP))
 
all: init $(TARGET)

tag:
	ctags -R src/

init:
	@mkdir -p $(DEPDIR)
	@mkdir -p $(OBJDIR)

main: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)
 
-include $(DEPS)
 
$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
	$(CC) -MM -MT $(OBJDIR)/$*.o $(CFLAGS) $(SRCDIR)/$*.cpp > $(DEPDIR)/$*.d

format:
	cd src && clang-format -style=WebKit -i *.h *.cpp
 
clean:
	rm -fr debug/*
	rm -fr release/*
