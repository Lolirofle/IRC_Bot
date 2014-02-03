CC=gcc

CFLAGS=-Wall -std=gnu99
CFLAGS_BIN=
CFLAGS_LIB=-fPIC
LDFLAGS=-llolie -largtable2
LDFLAGS_BIN=-ldl -Wl,-export-dynamic
LDFLAGS_LIB=

BINDIR=bin
OBJDIR=obj
SRCDIR=src
SRCDIR_BIN=ircbot
SRCDIR_LIB=ircinterface

OUT_BIN=ircbot
OUT_LIB=ircinterface

vpath %.c $(SRCDIR)
rwildcard=$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

SOURCES=
OBJECTS=$(SOURCES:./$(SRCDIR)/%.c=$(OBJDIR)/%.o)

SOURCES=$(call rwildcard,./$(SRCDIR)/$(SRCDIR_BIN),*.c)
OBJECTS_BIN:=$(OBJECTS)

SOURCES=$(call rwildcard,./$(SRCDIR)/$(SRCDIR_LIB),*.c)
OBJECTS_LIB:=$(OBJECTS)

OUTPREFIX_BIN=
OUTPOSTFIX_BIN=
OUTPREFIX_LIB=lib
OUTPOSTFIX_LIB=.so

LDFLAGS_LIB+= -shared -Wl,-soname,$(OUTPREFIX_LIB)$(OUT_LIB)$(OUTPOSTFIX_LIB)
LDFLAGS_BIN+= -lircinterface -L$(BINDIR)

all: CFLAGS+= -O3
all: $(OUT_LIB)
all: $(OUT_BIN)

debug: CFLAGS+= -g -ftrapv -Wundef -Wpointer-arith -Wcast-align -Wwrite-strings -Wcast-qual -Wswitch-default -Wunreachable-code -Wfloat-equal -Wuninitialized -Wignored-qualifiers -Wsuggest-attribute=pure -Wsuggest-attribute=const
debug: $(OUT_LIB)
debug: $(OUT_BIN)

$(OBJDIR)/$(SRCDIR_BIN)/%.o: $(SRCDIR_BIN)/%.c
	$(CC) -I$(SRCDIR)/ -I$(SRCDIR)/$(SRCDIR_BIN) $(CFLAGS) $(CFLAGS_BIN) -o $@ -c $<

$(OBJDIR)/$(SRCDIR_LIB)/%.o: $(SRCDIR_LIB)/%.c
	$(CC) -I$(SRCDIR)/$(SRCDIR_LIB) $(CFLAGS) $(CFLAGS_LIB) -o $@ -c $<

clean:
	rm -f $(OBJECTS_BIN) $(OBJECTS_LIB) $(BINDIR)/$(OUTPREFIX_BIN)$(OUT_BIN)$(OUTPOSTFIX_BIN) $(BINDIR)/$(OUTPREFIX_LIB)$(OUT_LIB)$(OUTPOSTFIX_LIB)

$(OUT_BIN): $(OBJECTS_BIN)
	$(CC) -o $(BINDIR)/$(OUTPREFIX_BIN)$@$(OUTPOSTFIX_BIN) $(LDFLAGS) $(LDFLAGS_BIN) $^

$(OUT_LIB): $(OBJECTS_LIB)
	$(CC) -o $(BINDIR)/$(OUTPREFIX_LIB)$@$(OUTPOSTFIX_LIB) $(LDFLAGS) $(LDFLAGS_LIB) $^

run:
	cd $(BINDIR) && ./$(OUTPREFIX_BIN)$(OUT_BIN)$(OUTPOSTFIX_BIN)
