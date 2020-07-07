# Shell commands
RM := rm -rf

# Compiler setting (default it will be gcc)
CC ?= gcc

# Enable max optimization
CC_OPT := -O3

# Maybe some flags are duplicated, but who cares
CC_WARNINGS := -mavx -Wall -Wextra -pedantic -Wcast-align \
               -Winit-self -Wmissing-include-dirs \
               -Wredundant-decls -Wshadow -Wstrict-overflow=5 \
               -Wundef -Wwrite-strings -Wpointer-arith \
               -Wmissing-declarations -Wuninitialized \
               -Wold-style-definition -Wstrict-prototypes \
               -Wmissing-prototypes -Wswitch-default \
               -Wbad-function-cast -Wnested-externs \
               -Wconversion -Wunreachable-code \

ifeq ($(CC), $(filter $(CC), gcc cc))
	CC_SYM := -rdynamic
	CC_STD := -std=gnu99
else ifeq ($(CC),clang)
	CC_SYM := -Wl, --export-dynamic
	CC_WARNINGS += -Wgnu -Weverything -Wno-newline-eof \
               -Wno-unused-command-line-argument \
               -Wno-reserved-id-macro -Wno-documentation \
               -Wno-documentation-unknown-command \
               -Wno-padded
	CC_STD := -std=c99
endif

CC_FLAGS := $(CC_STD) $(CC_WARNINGS) $(CC_OPT) $(CC_SYM)

PROJECT_DIR := $(shell pwd)

# To enable verbose mode type make V =1
ifeq ("$(origin V)", "command line")
	VERBOSE = $(V)
endif

ifndef VERBOSE
	VERBOSE = 0
endif

ifeq ($(VERBOSE), 1)
	Q =
else
	Q = @
endif

define print_info
	$(if $(Q), @echo "$(1)")
endef

define print_make
	$(if $(Q), @echo "[MAKE] $(1)")
endef

define print_cc
	$(if $(Q), @echo "[CC]   $(1)")
endef 

define print_bin
	$(if $(Q), @echo "[BIN]  $(1)")
endef

IDIR := $(PROJECT_DIR)/inc
SDIR := $(PROJECT_DIR)/src
TDIR := $(PROJECT_DIR)/test

SRCS := $(wildcard $(SDIR)/*.c) $(wildcard $(TDIR)/*.c)
OBJS := $(SRCS:%.c=%.o)
DEPS := $(wildcard $(IDIR)/*.h)

# Put here all needed libraries like math, pthread etc
LIBS := -lm

# Type here name of your output file
EXEC := $(PROJECT_DIR)/main.out

all: $(EXEC)

%.o: %.c
	$(call print_cc, $<)
	$(Q)$(CC) $(CC_FLAGS) -I$(IDIR) -c $< -o $@

$(EXEC): $(OBJS)
	$(call print_bin, $@)
	$(Q)$(CC) $(CC_FLAGS) -I$(IDIR) $(OBJS) $(LIBS) -o $@

clean:
	$(call print_info,Cleaning)
	$(Q)$(RM) $(OBJS)
	$(Q)$(RM) $(EXEC)