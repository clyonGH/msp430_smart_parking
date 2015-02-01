CC=msp430-gcc
CFLAGS=-Os -Wall -mmcu=cc430f5137
TARGET=test_rf
SRCDIR=src
INCLDIR=include
RFDIR=rf

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

## Rule for making the actual target
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

deploy:$(TARGET)
	mspdebug rf2500 "prog $(TARGET)"
