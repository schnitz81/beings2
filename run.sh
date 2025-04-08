if ! [ -e beings ]; then
	echo "No executable found. Preparing to build..." 
	echo -n "Checking gcc..."
	if [ -z "$(which gcc)" ]; then
		echo -e "failed!\n\nMake sure gcc is installed.\n"
		exit 1
	fi
	echo "success!";

	echo -n "Checking curses..."
	if [[ ! -n $(find /usr/* -name 'ncurses.h') ]]; then
		echo -e "failed!\n\nncurses.h not found. Make sure ncurses package is installed.\n"
		exit 1
	fi
	echo "success!"

    echo "Proceeding to build executable..."

	echo 'Building...'
	gcc -Wfatal-errors -Wall -c main.c &&
	gcc -Wfatal-errors -Wall -c world.c &&
	gcc -Wfatal-errors -Wall -c ai.c &&
	gcc -Wfatal-errors -Wall -c event.c &&
	gcc -Wfatal-errors -Wall -c being.c &&
	gcc -Wfatal-errors -o beings main.o world.o ai.o event.o being.o -lncurses -ltinfo 

	if [ $? -eq 0 ]; then
		echo "Build successful. Starting executable..."
		./beings
	else
		echo "Build failed. Unable to start."
	fi
	
else
	echo -e "\nExecutable found. Starting..."
	./beings
fi
