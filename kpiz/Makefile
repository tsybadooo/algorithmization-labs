# КПІЗ, варіант 19. Збирання: make (GCC, MSYS2/Linux), тести: make test
CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -pedantic -g
LDLIBS  = -lm
B       = build

.PHONY: all test memcheck cppcheck coverage clean

all: $(B)/part2.exe $(B)/part3.exe $(B)/part4.exe $(B)/part5.exe

$(B):
	mkdir -p $(B)

$(B)/part2.exe: part2/main.c part2/func.c part2/func.h | $(B)
	$(CC) $(CFLAGS) -o $@ part2/main.c part2/func.c $(LDLIBS)

$(B)/part3.exe: part3/main.c part3/matrix.c part3/sort.c part3/search.c | $(B)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(B)/part4.exe: part4/main.c part4/text.c | $(B)
	$(CC) $(CFLAGS) -o $@ $^

$(B)/part5.exe: part5/main.c part5/bus.c part5/storage.c | $(B)
	$(CC) $(CFLAGS) -o $@ $^

$(B)/test_assert.exe: tests/test_assert.c part2/func.c part3/matrix.c part3/sort.c part3/search.c part4/text.c | $(B)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

$(B)/test_cunit.exe: tests/test_cunit.c part3/matrix.c part3/sort.c part3/search.c part4/text.c | $(B)
	$(CC) $(CFLAGS) -o $@ $^ -lcunit

test: $(B)/test_assert.exe $(B)/test_cunit.exe
	./$(B)/test_assert.exe
	./$(B)/test_cunit.exe

# На Linux: valgrind --leak-check=full --show-leak-kinds=all ./build/part5.exe
memcheck: | $(B)
	$(CC) $(CFLAGS) -c common/memtrack.c -o $(B)/memtrack.o
	$(CC) $(CFLAGS) -DMEMTRACK -include common/memtrack.h -o $(B)/tests_mt.exe \
	    tests/test_assert.c part2/func.c part3/matrix.c part3/sort.c part3/search.c part4/text.c $(B)/memtrack.o $(LDLIBS)
	./$(B)/tests_mt.exe

cppcheck:
	cppcheck --enable=warning,style,performance,portability --std=c11 --inline-suppr part2 part3 part4 part5 tests

coverage: | $(B)
	cd $(B) && $(CC) -std=c11 -O0 --coverage -o cov.exe ../tests/test_assert.c ../part2/func.c \
	    ../part3/matrix.c ../part3/sort.c ../part3/search.c ../part4/text.c $(LDLIBS) && ./cov.exe && gcov -b -f cov-matrix.gcda

clean:
	rm -rf $(B)
