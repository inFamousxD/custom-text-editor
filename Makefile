texteditor: src/texteditor.c
	$(CC) src/texteditor.c -o dist/texteditor -Wall -Wextra -pedantic -std=c99
	./dist/texteditor