// REQUIREMENTS
// X the executable for this program should be named 'mygrep'
// X not allowed to use the functions: fgets(), gets(), scanf(), or fscanf()
// X all terminal output to be done with printf() or fprintf()
// X all error messages should go to stderr, not stdout
// X must error check the original call and all library calls
// X in the event of a error with a library call, print the standard system
//    message using perror() or strerror()
//    e.g. mygrep: cannot open file: no such file
// X return code should be same as grep: 0 if line is selected, 1 if no line
//    is selected, 2 if error occurred - this behavior is pretty much the
//    same when the '-v' or '--invert-match' option is used, however what
//    happens is: 0 if there are any lines that do not match, 1 if all lines
//    match, and 2 if there is an error - this has been tested with the
//    original grep program - in effect, either with or without the '-v' or
//    '--invert-match' option, the return codes are: 0 if any lines are
//    printed, 1 if no lines are printed, 2 if there is an error
//    note: the return code 2 occurs in grep regardless of match or no match if
//           an error occurs - this program behaves the same

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
// unistd.h is included for the access() function and associated constants
//  the access() function tests for file existence and permissions
#include<unistd.h>
// errno.h is included for the errno variable that is set by system calls and
//  library functions when an error occurs - for use with perror() and
//  strerror()
#include <errno.h>


// preprocessor directives for exit codes
#define R_MATCH 0
#define R_NOMATCH 1
#define R_ERROR 2
// preprocessor directive for program name
#define PROG_NAME "mygrep"

// FUNCTION PROTOTYPES
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert,
 int printfname);
char *get_next_line(FILE *fpntr);
void freestrarr(int size, char **arr);
void printusage(char *progname);
void removestr(char **arr, int index, int len);
void printstrarr(char **arr, int len);

// main function
int main(int argc, char *argv[]) {
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// START VARIABLE DECLARATIONS
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// boolean for inverse match
	int invert = 0;
	// boolean for reading stdin instead of file
	int readstdin = 0;
	// file handle for open file
	FILE *fileh;
	// search string from argument
	char *searchstr;
	// file paths from argument
	char **filenames;
	// index of current position in argv - start from index 1 because index 0 is
	//  executable name
	int argidx = 1;
	// number of files given in args
	int numfiles;
	// index for filenames
	int fidx = 0;
	// length of each filename arg
	int arglen;
	// keep track of number of matches overall
	int foundmatch = 0;
	// keep track of number of errors overall
	int founderror = 0;
	// boolean to signal if filename should be prepended before matched
	//  lines. this should only happen if more than one filename is given in
	//  args
	int printfname = 0;
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// END VARIABLE DECLARATIONS
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// START ARGUMENT PARSING
	// examine the command line args to get search string, check for
	//  invert option '-v' or '--invert-match', and determine if reading
	//  a file or from stdin
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// if there are no arguments, print usage message and exit with error status
	if ( argc == 1 ) {
		printusage(PROG_NAME);
		return(R_ERROR);
	}
	// if the first argument is invert option, set invert boolean and
	//  increment arg index to next position
	// note: if invert option is specified, it must be the first argument
	if ( strcmp(argv[argidx],"-v") == 0 ||
		 strcmp(argv[argidx],"--invert-match") == 0 ) {
		// if the option is specified, set the boolean flag and
		//  increment the arg index
		invert = 1;
		argidx++;
	}
	// get the search string from arg
	searchstr = argv[argidx];
	// go to the next argument - start of the file paths
	argidx++;
	// calculate how many filenames there are
	numfiles = argc - argidx;
	// if there are filename args given, read them in
	if (numfiles > 0) {
		// if more than one file is given in args, set flag to print
		//  filename before matched lines
		if (numfiles > 1) { printfname = 1; }
		// allocate memory in filename array for each file arg
		filenames = malloc(numfiles * sizeof(char *));
		// if error allocating memory, print error and exit
		if (filenames == NULL) {
			fprintf(stderr,"%s: error allocating memory: %s\n",
				PROG_NAME,strerror(errno));
			founderror++;
			return(R_ERROR);
		}
		// get the file paths
		for (; argidx < argc; argidx++) {
			// find length of arg
			arglen = strlen(argv[argidx]);
			// allocate some memory to store arg in filename array
			filenames[fidx] = (char *) malloc((arglen+1) * 
				sizeof(char));
			// if error allocating memory, print error and exit
			if (filenames == NULL) {
				fprintf(stderr,"%s: error allocating memory: %s"
					"\n",PROG_NAME,strerror(errno));
				founderror++;
				return(R_ERROR);
			}
			// copy argument to filename array
			strcpy(filenames[fidx],argv[argidx]);
			// increment index of filename array
			fidx++;
		}
	}
	// if there are no filename args, assume stdin (set flag)
	else { readstdin = 1; }
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// END ARGUMENT PARSING
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// START ARGUMENT CHECKING
	// if there is an error with the arguments, output should be a
	//  usage message
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// check if search string was given in args
	if (searchstr == NULL || searchstr[0] == '\0') {
		// if no search string given, print usage and exit
		printusage(PROG_NAME);
		founderror++;
		return(R_ERROR);
	}
	// check each filename given in args for existence and readability
	if (numfiles > 0) {
		for (fidx=0; fidx<numfiles; fidx++) {
			// check if file exists
			if (access(filenames[fidx],F_OK) == 0) {
				// if file is not readable, print error and
				//  remove filename from array
				if (access(filenames[fidx],R_OK) != 0) {
					fprintf(stderr,"%s: file '%s' is not "
						"readable: %s\n",PROG_NAME,
						filenames[fidx],
						strerror(errno));
					// remove unreadable file from array
					removestr(filenames,fidx,numfiles);
					// decrement number of files
					numfiles--;
					// move array index back by one since
					//  the current element was removed
					fidx--;
					// increment the error counter
					founderror++;
				}
			}
			// if file doesn't exist, print error and remove
			//  filename from array
			else {
				fprintf(stderr,"%s: file '%s' does not exist: "
					"%s\n",PROG_NAME,filenames[fidx],
					strerror(errno));
				// remove unreadable file from array
				removestr(filenames,fidx,numfiles);
				// decrement number of files
				numfiles--;
				// move the array index back by one since the
				//  current element was removed
				fidx--;
				// increment the error counter
				founderror++;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// END ARGUMENT CHECKING
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	

	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// START STREAM PROCESSING
	// if a file is to be processed, open it. otherwise use stdin
	// call function to process the stream
	// when function returns, close stream if it was a file
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// temporary storage for number of matches
	int grepreturn;
	// if no files were given, read from stdin
	if (readstdin) {
		// look for string match in stdin
		grepreturn = grep_stream(stdin,searchstr,NULL,invert,
			printfname);
		// if error in grep, print error and exit
		if (grepreturn == -1) {
			fprintf(stderr,"%s: problem finding match: %s\n",
				PROG_NAME,strerror(errno));
			return(R_ERROR);
		}
		// if no error, add to foundmatch
		else { foundmatch += grepreturn; }
	}
	else {
		// loop through each file given in args
		for(fidx=0; fidx<numfiles; fidx++) {
			// open the file for reading only
			fileh = fopen(filenames[fidx],"r");
			// if there is a problem opening, skip to next iteration
			//  of loop
			if ( fileh == NULL) {
				fprintf(stderr,"%s: file '%s' failed to open: "
					"%s\n",PROG_NAME,filenames[fidx],
					strerror(errno));
				continue;
			}
			// look for string match in file
			grepreturn = grep_stream(fileh,searchstr,
				filenames[fidx],invert,printfname);
			// if error in grep, print error
			if (grepreturn == -1) {
				fprintf(stderr,"%s: problem finding match in "
					"file '%s': %s\n",PROG_NAME,
					filenames[fidx],strerror(errno));
			}
			else { foundmatch += grepreturn; }
			// close the file, printing error if unsuccessful
			if (fclose(fileh) != 0) {
				fprintf(stderr,"%s: file '%s' failed to close: "
					"%s\n",PROG_NAME,filenames[fidx],
					strerror(errno));
			}
		}
	}
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// END STREAM PROCESSING
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// START CLEANUP AND RETURN
	// free any allocated memory and exit with appropriate exit status
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// if not reading from stdin, assume that memory was allocated for array
	//  of filenames, so free the memory
	if (! readstdin) { freestrarr(numfiles,filenames); }
	// return appropriate code based on if match was found or any errors
	//  occurred
	if (founderror>0) { return(R_ERROR); }
	else if (foundmatch>0) { return(R_MATCH); }
	else { return(R_NOMATCH); }
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// END CLEANUP AND RETURN
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
}

// fpntr is an open file stream
// string is the search string
// file_pathname is the file path to read, or stdin
// invert is boolean to invert match or normal match
// printfname is boolean to print filenames before matching lines if more than
//  one file is given in args
// reads line-by-line through the file, matches lines based on the search
//  string, prints them to stdout, returns true if any matching lines are
//  found, returns false if no matching lines were found
// note: this function should always return, never calling exit()
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert,
	int printfname) {
	// initialize return code to zero
	// increments by 1 if line is matched
	int returncode = 0;
	// string to store each line
	char *line;
	// iteratively call function to get next line from the stream
	while (feof(fpntr) == 0) {
		// get next line from stream
		line = get_next_line(fpntr);
		// if there was a problem getting next line, print error and
		//  return with negative error code to indicate problem to
		//  calling function
		if (line == NULL) {
			fprintf(stderr,"%s: error reading line from file: %s\n",
				PROG_NAME,strerror(errno));
			return(-1);
		}
		// for each returned line, check if it contains the search string
		if (strstr(line,string) != NULL) {
			// if the line should be printed, print to stdout
			if (invert != 1) {
				// increment return code to indicate match
				returncode++;
				// if more than one file specified, print
				//  filename before matched line
				if (printfname) { printf("%s:%s\n",
					file_pathname,line); }
				// otherwise, just print matched line
				else { printf("%s\n",line); }
			}
		}
		// if line doesn't contain search string
		else {
			// print lines that do not match if the invert option
			//  specified
			if (invert == 1) {
				// increment return code to indicate non-match
				returncode++;
				// if more than one file specified, print
				//  filename before non-matching line
				if (printfname) { printf("%s:%s\n",
					file_pathname,line); }
				// otherwise, just print non-matchingline
				else { printf("%s\n",line); }
			}
		}
		// free allocated memory
		free(line);
	}
	// return number of matched or non-matching lines
	return(returncode);
}


// fpntr is an open file stream
// read the next line from a stream and return it as a string, returns NULL if
//  an I/O error occurs or the end of the file is reached
// note: the newline character is not considered part of the line and is
//        therefore not returned by this function
// note: this function should not print any error messages or other output and
//        it must always return
char *get_next_line(FILE *fpntr) {
	// current size of the buffer
	int buffsize = 1024;
	// char array to store line to return initialized with current buffer
	//  size
	char *line = malloc((buffsize+1) * sizeof(char));
	// return null from the function if malloc fails
	if (line == NULL) { return(NULL); }
	// int to store each char returned from stream
	int currchar;
	// counter for place in buffer
	int count = 0;

	// get the next character from the file
	currchar = fgetc(fpntr);
	// if there was an error, return null from function
	if (ferror(fpntr) != 0 ) { return(NULL); }
	// iteratively call fgetc() to obtain the next character from
	//  stream until the end of the current line is detected, the end of the
	//  file is reached, or error occurs when reading
	// note that fgetc() has an int return type due to the possibility
	//  of errors or EOF (normally -1) - the difference can be
	//  distinguished using ferror() or feof() or even errno
	while (currchar != (int) '\n' && feof(fpntr) == 0 && ferror(fpntr) == 0)
	{
		// if the buffer size has been reached, increase the size of the
		//  buffer and then add the char
		if (count == buffsize) {
			// double the buffer size
			buffsize *= 2;
			// reallocate memory up to the new buffer size
			line = realloc(line, buffsize);
			// return null from the function if realloc fails
			if (line == NULL) { return(NULL); }
			// put the returned char (after typecasting) at the next
			//  position in the buffer
			line[count] = (char) currchar;
		}
		// otherwise, just add the char to the buffer
		else {
			// put the returned char (after typecasting) at the next
			//  position in the buffer
			line[count] = (char) currchar;
		}
		// increment the buffer position counter
		count++;
		// get the next character from the file
		currchar = fgetc(fpntr);
		// if there was an error, return null from function
		if (ferror(fpntr) != 0 ) { return(NULL); }
	}
	// stringify the buffer by adding null char at the end of the chars
	line[++count] = '\0';
	// return next line in the stream as a string
	return line;
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


// progname is the designated name of this program
// prints usage message
void printusage(char *progname) {
	fprintf(stderr,"Usage: %s [-v|--invert-match] PATTERN [FILE]...\n",
		progname);
}


// arr is array of strings
// index is index to remove
// len is length of the array
// removes string from array at index
void removestr(char **arr, int index, int len) {
	// index to keep track of position in array
	int idx;
	// loop through array starting at given index, moving each element back
	//  by one until the next-from-last element is reached
	for (idx=index; idx<len-1; idx++) { arr[idx] = arr[idx+1]; }
}

// arr is array of strings
// len is length of array
// prints the array of strings, one per line
void printstrarr(char **arr, int len) {
	// index to keep track of position in array
	int idx;
	// loop through array from beginning to end, printing each element on a
	//  separate line as a string
	for (idx=0; idx<len; idx++) { printf("%s\n",arr[idx]); }
}
