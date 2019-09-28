all:
	arm-linux-gcc main.c -o miyooctl -ggdb -lSDL -Os -flto 
clean:
	rm -rf daemon
