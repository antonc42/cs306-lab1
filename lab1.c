#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// REQUIREMENTS
// XXX: the executable for this program should be named 'mygrep'
// XXX: not allowed to use the functions: fgets(), gets(), scanf(), or fscanf()
// XXX: all terminal output to be done with printf() or fprintf()
// XXX: all error messages should go to stderr, not stdout
// XXX: must error check the original call and all library calls
// XXX: in the event of a error with a library call, print the standard system
//      message using perror() or strerror()
//      e.g. mygrep: cannot open file: no such file
// XXX: return code should be same as grep - 0 if line is selected, 1 if no line
//      is selected, 2 if error occurred
// XXX: can set static buffer size of 512 or 1024 or something using
//      preprocessor directive like "#define BUFFER_SIZE 512"

// FUNCTION PROTOTYPES
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert);
char *get_next_line(FILE *fpntr);
void freestrarr(int size, char **arr);


int main(int argc, char *argv[]) {
	// boolean for inverse match
	int invert = 0;
	// file handle for open file
	FILE *fileh;
	// search string from argument
	char *searchstr;
	// file paths from argument
	char **filenames;
	// index of current position in argv - start from index 1 because index 0 is executable name
	int argidx = 1;

	// TODO examine the command line args to get search string, check for
	//      invert option '-v' or '--invert-match', and determine if reading
	//      a file or from stdin
	// note: if invert option is specified, it must be the first argument
	if ( strcmp(argv[argidx],"-v") == 0 || strcmp(argv[argidx],"--invert-match") == 0 ) {
		invert = 1;
		argidx++;
	}
	// get the search string
	searchstr = argv[argidx];
	// go to the next argument - start of the file paths
	argidx++;
	// calculate how many filenames there are
	int numfiles = argc - argidx;
	// allocate memory in filename array for each file arg
	filenames = malloc(numfiles * sizeof(char *));
	// index for filenames
	int fidx = 0;
	// length of each arg
	int arglen;
	// get the file paths
	for (; argidx < argc; argidx++) {
		// find length of arg
		arglen = strlen(argv[argidx]);
		// allocate some memory to store arg in filename array
		filenames[fidx] = (char *) malloc((arglen+1) * sizeof(char));
		// copy argument to filename array
		strcpy(filenames[fidx],argv[argidx]);
		// increment index of filename array
		fidx++;
	}
	printf("%d\n",numfiles);
	// print the file paths
	for (fidx=0; fidx<numfiles; fidx++) {
		printf("%s\n",filenames[fidx]);
	}
	
	// TODO if there is an error with the arguments, output should be a
	//      usage message
	
	// TODO if a file is to be processed, open it. otherwise use stdin
	
	// TODO call function to process the stream
	// current filename
	//char *curfile;
	// TODO for loop here
	//	grep_stream(fileh,searchstr,curfile,invert);
	
	// free the memory allocated for the array of filenames
	freestrarr(numfiles,filenames);
	// TODO when function returns, close stream if it was a file
	
	// TODO exit with appropriate exit status
	return EXIT_SUCCESS;	
}

// fpntr is an open file stream
// string is the search string
// file_pathname is the file path to read, or stdin
// reads line-by-line through the file, matches lines based on the search
//  string, prints them to stdout, returns true if any matching lines are
//  found, returns false if no matching lines were found
// XXX: this function should always return, never calling exit()
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert) {
	// initialize return code to false
	// gets changed to true if any lines are matched
	int returncode = 0;
	// TODO iteratively call function to get next line from the stream
	
	// TODO for each returned line, check if it contains the search string
	
	// TODO if the line should be printed, print to stdout
	if (invert == 1) {
		// TODO print only lines that do not match
	}
	else {
		// TODO print only lines that match
	}
	
	// TODO when function returns NULL, return true or false depending on
	//      whether any matching lines were found - true if any found, false
	//      if none found
	return returncode;
}


// fpntr is an open file stream
// read the next line from a stream and return it as a string, returns NULL if
//  an I/O error occurs or the end of the file is reached
// note: the newline character is not considered part of the line and is
//       therefore not returned by this function
// XXX: this function should not print any error messages or other output and
//      it must always return
char *get_next_line(FILE *fpntr) {
	char *nextline;
	// TODO iteratively call fgetc() to obtain the next character from
	//      stream until the end of the current line is detected or error
	//      occurs when reading
	// XXX  note that fgetc() has an int return type due to the possibility
	//      of errors or EOF (normally -1) - the difference can be
	//      distinguished using ferror() or feof() or even errno
	// XXX  an int array should be used to read in characters because of EOF
	//      it can always be typecast back to char
	
	// TODO store each read char into a line buffer array created with
	//      malloc() - remember to allocate an extra byte at the end for the
	//      null character '\0'
	
	// TODO return NULL if there are no more lines or if error occurred, do
	//      not print an error here - calling function should print error
	
	// TODO when the end of the line is reached, turn the buffer array into
	//      a valid C string and return it

	// return next line in the stream as a string
	return nextline;
}


// size is number of elements in string array
// arr is the string array
// frees allocated memory for a string array
void freestrarr(int size, char **arr) {
	int idx;
	for (idx=0; idx < size; idx++) {
		free(arr[idx]);
	}
	free(arr);
}
