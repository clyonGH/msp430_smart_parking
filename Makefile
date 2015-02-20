CC=msp430-gcc
CFLAGS=-Os -Wall -mmcu=cc430f5137
TARGET=test_rf
SRCDIR=src
INCLDIR=include
RFDIR=rf
DEFINES=

SOURCE=$(wildcard $(SRCDIR)/*.c)
SOURCE+=$(wildcard $(RFDIR)/*.c)

## End sources definition
INCLUDE=-I$(INCLDIR) -I$(RFDIR)
## end more includes

OBJ=$(SOURCE:.c=.o)

## Default rule executed
all: $(TARGET)
	@true

## Clean Rule
clean:
	echo $(OBJ)
	@-rm -f $(TARGET) $(OBJ)

## Transmission
tx: DEFINES += -DTX=1
tx: DEFINES += -DRX=0
tx: DEFINES += -DID_DEVICE=1
tx: DEFINES += -DNEXT_DEVICE=1
tx: DEFINES += -DID_SRC=$(SRC)
tx: DEFINES += -DID_DEST=$(DEST)
tx: $(TARGET)

## Reception
rx: DEFINES += -DTX=0
rx: DEFINES += -DRX=1
rx: DEFINES += -DID_SRC=1
rx: DEFINES += -DID_DEST=1
rx: DEFINES += -DID_DEVICE=$(ID)
rx: DEFINES += -DNEXT_DEVICE=$(NEXT)
rx: $(TARGET)

## Rule for making the actual target
$(TARGET): CFLAGS += $(DEFINES)
$(TARGET): $(OBJ)
	@echo "============="
	@echo "Linking the target $@"
	@echo "============="
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	@echo -- Link finished --

## Generic compilation rule
%.o : %.c
	@mkdir -p $(dir $@)
	@echo "============="
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

deploy:
	mspdebug rf2500 "prog $(TARGET)"
