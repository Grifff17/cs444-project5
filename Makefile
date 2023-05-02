CCOPTS=-Wall -Wextra

.PHONY: test clean

simfs.a: block.o image.o free.o inode.o mkfs.o
	ar rcs $@ $^

block.o: block.c block.h
	gcc $(CCOPTS) -c $<

image.o: image.c image.h
	gcc $(CCOPTS) -c $<

free.o: free.c free.h
	gcc $(CCOPTS) -c $<

inode.o: inode.c inode.h
	gcc $(CCOPTS) -c $<

mkfs.o: mkfs.c mkfs.h
	gcc $(CCOPTS) -c $<

simfs_test: simfs_test.c simfs.a
	gcc $(CCOPTS) -DCTEST_ENABLE -o $@ $^

test: simfs_test
	./simfs_test

clean:
	rm -f *.o *.a *_testfile simfs_test
