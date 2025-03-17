#if ! [ -e beings ]; then
	echo "No executable found. Preparing to build..."; sleep .1
	echo -n "Checking gcc..."; sleep .1
	if [ -z "$(which gcc)" ]; then
		echo -e "failed!\n\nMake sure gcc is installed.\n"
		exit 1
	fi


    echo "Proceeding to build executable..."; sleep .1

	echo 'Building...'
	gcc -v -Wfatal-errors -Wall -c main.c &&
	gcc -v -Wfatal-errors -Wall -c world.c &&
	gcc -v -Wfatal-errors -Wall -c ai.c &&
	gcc -v -Wfatal-errors -Wall -c event.c &&
	gcc -v -Wfatal-errors -Wall -c being.c &&
	gcc -v -Wfatal-errors -o beings main.o world.o ai.o event.o being.o -lncurses -ltinfo 

	if [ $? -eq 0 ]; then
		echo "Build successful."; sleep 1
		#./beings
	else
		echo "Build failed."
	fi
	
#else
#	echo -e "\nExecutable found. Starting..."; sleep 1
	#./beings
#fi



