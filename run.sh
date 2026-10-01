if ! [ -e beings2 ]; then
	echo "No executable found. Preparing to build..."
	echo -n "Checking gcc..."
	if [ -z "$(which gcc)" ]; then
		echo -e "failed!\n\nMake sure gcc is installed.\n"
		exit 1
	fi
	echo "success!"

	echo -n "Checking curses..."
	if printf "#include <curses.h>\n" | gcc -E -x c - >/dev/null 2>&1; then
		echo "success!"
	else
		echo -e "failed!\n\ncurses.h header file not found. Make sure curses (most often ncurses) is installed.\n"; exit 1
	fi

	echo "Proceeding to build executable..."

	echo 'Building...'
	gcc -Wfatal-errors -Wall -c main.c &&
	gcc -Wfatal-errors -Wall -c world.c &&
	gcc -Wfatal-errors -Wall -c ai.c &&
	gcc -Wfatal-errors -Wall -c event.c &&
	gcc -Wfatal-errors -Wall -c being.c &&
	gcc -Wfatal-errors -o beings2 main.o world.o ai.o event.o being.o -lncurses -ltinfo

	if [ $? -eq 0 ]; then
		echo "Build successful. Starting executable..."
		./beings2
	else
		echo "Build failed. Unable to start."
	fi

else
	echo -e "\nExecutable found. Starting..."
	./beings2
fi
