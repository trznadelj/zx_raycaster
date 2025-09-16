Fast raycasting on zx spectrum, achieving around 50 FPS. Actually playable.

To compile: 
   zcc.exe +zx -vn -O2 gfx.c -o gfx.bin -lndos -create-app -lm -m -s

Not everything is cleaned up, sorry for that.


python correlate.py --base 0 --include-token code_compiler gfx.map profile_from_fuse
