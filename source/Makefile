CC=../bin/zcc.exe
ASM=../bin/z80asm.exe
APPMAKE=../bin/z88dk-appmake.exe

all:
	$(CC) +zx -vn -O2 gfx.c -o gfx.bin -lndos -create-app -lm -m -s
	$(CC) +zx -vn -O2 gfx.c -o gfx.bin -lndos -lm -m -s
	./z88dk-dis.exe gfx.bin > gfx.asm


link_sdcc:
	$(CC) +zx -compiler=sdcc -vn -O2 -c gfx.c -o gfx.o -m -s
	$(ASM) -ogfx.bin -b gfx.o crt0.asm -l./z80.lib
	$(APPMAKE) +zx -b gfx.bin -o gfx.tap