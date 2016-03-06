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

// REQUIREMENTS
// XXX: the executable for this program should be named 'mygrep'
// XXX: not allowed to use the functions: fgets(), gets(), scanf(), or fscanf()
// XXX: all terminal output to be done with printf() or fprintf()
// XXX: all error messages should go to stderr, not stdout
// XXX: must error check the original call and all library calls
// XXX: in the event of a error with a library call, print the standard system
//       message using perror() or strerror()
//       e.g. mygrep: cannot open file: no such file
// XXX: return code should be same as grep: 0 if line is selected, 1 if no line
//       is selected, 2 if error occurred - this behavior is pretty much the
//       same when the '-v' or '--invert-match' option is used, however what
//       happens is: 0 if there are any lines that do not match, 1 if all lines
//       match, and 2 if there is an error - this has been tested with the
//       original grep program - in effect, either with or without the '-v' or
//       '--invert-match' option, the return codes are: 0 if any lines are
//       printed, 1 if no lines are printed, 2 if there is an error
// XXX: can set static buffer size of 512 or 1024 or something using
//       preprocessor directive like "#define BUFFER_SIZE 512"

// preprocessor directives for exit codes
#define R_MATCH 0
#define R_NOMATCH 1
#define R_ERROR 2
// preprocessor directive for program name
#define PROG_NAME "mygrep"

// FUNCTION PROTOTYPES
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert, int printfname);
char *get_next_line(FILE *fpntr);
void freestrarr(int size, char **arr);
void printusage(char *progname);


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
	// index of current position in argv - start from index 1 because index 0 is executable name
	int argidx = 1;
	// number of files given in args
	int numfiles;
	// index for filenames
	int fidx = 0;
	// length of each filename arg
	int arglen;
	// keep track of return code from grep_stream function to determine if
	//  any matches were found
	int foundmatch = 0;
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
	// TODO examine the command line args to get search string, check for
	//      invert option '-v' or '--invert-match', and determine if reading
	//      a file or from stdin
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// if there are no arguments, print usage message and exit with error status
	if ( argc == 1 ) {
		printusage(PROG_NAME);
		return(R_ERROR);
	}
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
	// FIXME debugging
	//printf("search: %s\n",searchstr);
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
		// get the file paths
		for (; argidx < argc; argidx++) {
			// find length of arg
			arglen = strlen(argv[argidx]);
			// allocate some memory to store arg in filename array
			filenames[fidx] = (char *) malloc((arglen+1) * 
				sizeof(char));
			// copy argument to filename array
			strcpy(filenames[fidx],argv[argidx]);
			// increment index of filename array
			fidx++;
		}
		// FIXME debugging
		//printf("number of files: %d\n",numfiles);
		// FIXME degugging - print the file paths
		//for (fidx=0; fidx<numfiles; fidx++) {
		//	printf("%s\n",filenames[fidx]);
		//}
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
	// TODO if there is an error with the arguments, output should be a
	//      usage message
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// check if search string was given in args
	if (searchstr == NULL || searchstr[0] == '\0') {
		// if no search string given, print usage and exit
		printusage(PROG_NAME);
		exit(R_ERROR);
	}
	// check each filename given in args for existence and readability
	if (numfiles > 0) {
		for (fidx=0; fidx<numfiles; fidx++) {
			// check if file exists
			if (access(filenames[fidx],F_OK) == 0) {
				// if file is not readable, print error and exit
				if (access(filenames[fidx],R_OK) != 0) {
					fprintf(stderr,"%s: file '%s' is not "
						"readable: %s\n",PROG_NAME,
						filenames[fidx],
						strerror(errno));
					return(R_ERROR);
				}
			}
			// if file doesn't exist, print error and exit
			else {
				fprintf(stderr,"%s: file '%s' does not exist: "
					"%s\n",PROG_NAME,filenames[fidx],
					strerror(errno));
				return(R_ERROR);
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
	// TODO if a file is to be processed, open it. otherwise use stdin
	// TODO call function to process the stream
	// TODO when function returns, close stream if it was a file
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// if no files were given, read from stdin
	if (readstdin) {
		// look for string match in stdin
		foundmatch += grep_stream(stdin,searchstr,NULL,invert,printfname);
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
			// FIXME debugging
			//printf("foundmatch before '%s': %d\n",filenames[fidx],foundmatch);
			// look for string match in file
			foundmatch += grep_stream(fileh,searchstr,
				filenames[fidx],invert,printfname);
			// FIXME debugging
			//printf("foundmatch after: %d\n",foundmatch);
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
	// TODO free any allocated memory
	// TODO exit with appropriate exit status
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// if not reading from stdin, assume that memory was allocated for array
	//  of filenames, so free the memory
	if (! readstdin) { freestrarr(numfiles,filenames); }
	// return appropriate code based on if match was found
	if (foundmatch>0) { return(R_MATCH); }
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
// XXX: this function should always return, never calling exit()
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert,
	int printfname) {
	// initialize return code to false
	// gets changed if any lines are matched
	int returncode = 0;
	// string to store each line
	char *line;
	// TODO iteratively call function to get next line from the stream
	//      use strcopy to copy return to string
	while (feof(fpntr) == 0) {
		line = get_next_line(fpntr);
		if (line == NULL) {
			fprintf(stderr,"%s: error reading line from file: %s\n",
				PROG_NAME,strerror(errno));
			return(R_ERROR);
		}
		// TODO for each returned line, check if it contains the search string
		if (strstr(line,string) != NULL) {
			// TODO if the line should be printed, print to stdout
			if (invert != 1) {
				returncode = 1;
				if (printfname) { printf("%s:%s\n",file_pathname,line); }
				else { printf("%s\n",line); }
			}
		}
		else {
			// TODO print only lines that do not match
			if (invert == 1) {
				returncode = 1;
				if (printfname) { printf("%s:%s\n",file_pathname,line); }
				else { printf("%s\n",line); }
			}
		}
		free(line);
	}
	// FIXME debugging
	//printf("greping for '%s' from file '%s'\n",string,file_pathname);
	
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
	// current size of the buffer
	int buffsize = 1024;
	// char array to store line to return
	//  initialize with current buffer size
	char *line = malloc((buffsize+1) * sizeof(char));
	// return null from the function if malloc fails
	if (line == NULL) { return(NULL); }
	// int to store each char returned from stream
	int currchar = 0;
	// counter for place in buffer
	int count = 0;

	// get the next character from the file
	currchar = fgetc(fpntr);
	// if there was an error, return null from function
	if (ferror(fpntr) != 0 ) { return(NULL); }
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
	while (currchar != (int) '\n' && feof(fpntr) == 0 && ferror(fpntr) == 0)
	{
		// if the buffer size has been reached, increase the size of the
		//  buffer and then add the char
		if (count == buffsize) {
			buffsize *= 2;
			line = realloc(line, buffsize);
			// return null from the function if realloc fails
			if (line == NULL) { return(NULL); }
			line[count] = (char) currchar;
		}
		// otherwise, just add the char to the buffer
		else {
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


// prints usage message
void printusage(char *progname) {
	fprintf(stderr,"Usage: %s [-v|--invert-match] PATTERN [FILE]...\n",progname);
}
