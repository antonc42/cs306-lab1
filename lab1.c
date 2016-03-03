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

// FUNCTION PROTOTYPES
int grep_stream(FILE *fpntr, char *string, char *file_pathname);
char *get_next_line(FILE *fpntr);


int main(int argc, char *argv[]) {
	// TODO examine the command line args to get search string, check for
	//      invert option '-v' or '--invert-match', and determine if reading
	//      a file or from stdin
	// note: if invert option is specified, it must be the first argument
	
	// TODO if there is an error with the arguments, output should be a
	//      usage message
	
	// TODO if a file is to be processed, open it. otherwise use stdin
	
	// TODO call function to process the stream
	
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
int grep_stream(FILE *fpntr, char *string, char *file_pathname) {
	// initialize return code to false
	// gets changed to true if any lines are matched
	int returncode = 0;
	// TODO iteratively call function to get next line from the stream
	
	// TODO for each returned line, check if it contains the search string
	
	// TODO if the line should be printed, print to stdout
	
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
	//      note that fgetc() has an int return type due to the possibility
	//      of errors or EOF (normally -1) - the difference can be
	//      distinguished using ferror() or feof() or even errno
	
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
