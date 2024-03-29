TARGET = sish 
CC = cc
CFLAGS  = -ansi -g -Wall -Werror -Wextra 
RM = rm -f

default: $(TARGET)
all: default

$(TARGET): $(TARGET).o 
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).o 

$(TARGET).o: $(TARGET).c $(TARGET).h
	$(CC) $(CFLAGS) -c $(TARGET).c

clean:
	$(RM) $(TARGET) *.o 
