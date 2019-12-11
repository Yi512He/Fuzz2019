all: fuzz_new pty

fuzz_new: ./fuzz/fuzz.c
	gcc ./fuzz/fuzz.c -o ./fuzz/fuzz

pty: ./ptyjig/ptyjig_cross_platform.c
	gcc ./ptyjig/ptyjig_cross_platform.c -o ./ptyjig/pty

clean:
	rm ./fuzz/fuzz ./ptyjig/pty
