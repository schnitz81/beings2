if ! [ -e beings ]; then
	echo "No executable found. Preparing to build..."; sleep .6
	echo -n "Checking gcc..."; sleep .4
	if [ -z "$(which gcc)" ]; then
		echo -e "failed!\n\nMake sure gcc is installed.\n"
		exit 1
	fi
	echo "success!"; sleep .4

	echo -n "Checking curses..."; sleep .4
	if [[ ! -n $(find /usr/* -name 'curses.h') ]]; then
		echo -e "failed!\n\ncurses.h not found. Make sure curses (most often ncurses) is installed.\n"
		exit 1
	fi
	echo "success!"; sleep .4

    echo "Proceeding to build executable..."; sleep .6

	echo 'Building...'
	gcc -Wfatal-errors -Wall -c main.c &&
	gcc -Wfatal-errors -Wall -c world.c &&
	gcc -Wfatal-errors -Wall -c ai.c &&
	gcc -Wfatal-errors -Wall -c event.c &&
	gcc -Wfatal-errors -Wall -c being.c &&
	gcc -Wfatal-errors -o beings main.o world.o ai.o event.o being.o -lcurses -ltinfo 

	if [ $? -eq 0 ]; then
		echo "Build successful. Starting executable..."; sleep 1
		./beings
	else
		echo "Build failed. Unable to start."
	fi
	
else
	echo -e "\nExecutable found. Starting..."; sleep 1
	./beings
fi



