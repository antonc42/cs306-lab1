#include<stdio.h>
#include<stdlib.h>

// REQUIREMENTS
// * the executable for this program should be named 'mygrep'

// FUNCTION PROTOTYPES
int grep_stream(FILE *fpntr, char *string, char *file_pathname);
char *get_next_line(FILE *fpntr);


int main(int argc, char *argv[]) {
	
	return EXIT_SUCCESS;	
}

// fpntr is an open file stream
// string is the search string
// file_pathname is the file path to read, or stdin
// reads line-by-line through the file, matches lines based on the search
//  string, prints them to stdout, returns true if any matching lines are
//  found, returns false if no matching lines were found
int grep_stream(FILE *fpntr, char *string, char *file_pathname) {
	// initialize return code to false
	// gets changed to true if any lines are matched
	int returncode = 0;

	// return boolean - false if no matching lines were found
	return returncode;
}


// fpntr is an open file stream
// read the next line from a stream and return it as a string, returns NULL if
//  an I/O error occurs or the end of the file is reached
// note: the newline character is not considered part of the line and is
//  therefore not returned by this function
char *get_next_line(FILE *fpntr) {
	char *nextline;

	// return next line in the stream as a string
	return nextline;
}
