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
//    is selected, 2 if error occurred 
//    X this behavior is pretty much the same when the '-v' or '--invert-match'
//       option is used, however what happens is: 0 if there are any lines that
//       do not match, 1 if all lines match, and 2 if there is an error
//    X this has been tested with the original grep program
//    X in effect, either with or without the '-v' or '--invert-match' option,
//       the return codes are: 0 if any lines are printed, 1 if no lines are
//       printed, 2 if there is an error
//    X the return code 2 occurs in grep regardless of match or no match if
//       an error occurs - this program behaves the same

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
// preprocessor directive of initial buffer size
#define BUFF_SIZE 1024

// FUNCTION PROTOTYPES
// TODO make grep_stream match spec in pdf
//      maybe do getter/setter or something to check invert and printfname flags
// TODO also make return codes of grep_stream and get_next_line match spec in pdf
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert,
 int printfname);
char *get_next_line(FILE *fpntr);
void free_str_arr(int size, char **arr);
void print_usage(char *progname);
void remove_str(char **arr, int index, int len);
void print_str_arr(char **arr, int len);
void print_buffer(char *buff, int len);

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
	FILE *fileh = NULL;
	// search string from args
	char *searchstr = NULL;
	// file paths from args
	char **filenames = NULL;
	// index of current position in argv - start from index 1 because index
	//  0 is executable name
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
	// temporary storage for number of matches each time grep_stream is
	//  called
	int grepreturn;
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
	// if there are no arguments, print usage message and exit with error
	//  status
	if ( argc == 1 ) {
		print_usage(PROG_NAME);
		return(R_ERROR);
	}
	// if the first argument is invert option, set invert boolean and
	//  increment arg index to next position
	// note: if invert option is specified, it must be the first argument
	if ( strcmp(argv[argidx],"-v") == 0 ||
		 strcmp(argv[argidx],"--invert-match") == 0 ) {
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
		// allocate memory in filename array for each file arg
		filenames = malloc(numfiles * sizeof(char *));
		// if error allocating memory, print error and exit
		if (filenames == NULL) {
			fprintf(stderr,"%s: error allocating memory: %s\n",
				PROG_NAME,strerror(errno));
			// increment error counter
			founderror++;
			return(R_ERROR);
		}
		// loop through args to get the file paths
		for (; argidx < argc; argidx++) {
			// find length of arg
			arglen = strlen(argv[argidx]);
			// allocate some memory to store arg in filename array
			filenames[fidx] = malloc((arglen+1) * sizeof(char));
			// if error allocating memory, print error and exit
			if (filenames == NULL) {
				fprintf(stderr,"%s: error allocating memory: %s"
					"\n",PROG_NAME,strerror(errno));
				// increment error counter
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
	//  usage message or useful error message if it is a non-fatal error
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// check if search string was given in args
	if (searchstr == NULL || searchstr[0] == '\0') {
		// if no search string given, print usage and exit
		print_usage(PROG_NAME);
		// increment the error counter
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
					remove_str(filenames,fidx,numfiles);
					// decrement number of files
					numfiles--;
					// check for no valid files
					if (numfiles<1) {
						// print usage message
						print_usage(PROG_NAME);
						// increment error counter
						founderror++;
						// return with error code
						return(R_ERROR);
					}
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
				// remove nonexistent file from array
				remove_str(filenames,fidx,numfiles);
				// decrement number of files
				numfiles--;
				// check for no valid files
				if (numfiles<1) {
					// print usage message
					print_usage(PROG_NAME);
					// increment error counter
					founderror++;
					// return with error code
					return(R_ERROR);
				}
				// move the array index back by one since the
				//  current element was removed
				fidx--;
				// increment the error counter
				founderror++;
			}
		}
		// if more than one valid file is given in args, set flag to
		//  print filename before matched lines
		if (numfiles > 1) { printfname = 1; }
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
	// if no files were given, read from stdin
	if (readstdin) {
		// look for string match in stdin
		grepreturn = grep_stream(stdin,searchstr,NULL,invert,
			printfname);
		// if error in grep, print error and exit
		if (grepreturn == -1) {
			fprintf(stderr,"%s: problem finding match: %s\n",
				PROG_NAME,strerror(errno));
			// increment the error counter
			founderror++;
			return(R_ERROR);
		}
		// if no error, add to match counter
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
				// increment error counter
				founderror++;
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
				// increment error counter
				founderror++;
			}
			// if no error in grep, add to match counter
			else { foundmatch += grepreturn; }
			// close the file, printing error if unsuccessful
			if (fclose(fileh) != 0) {
				fprintf(stderr,"%s: file '%s' failed to close: "
					"%s\n",PROG_NAME,filenames[fidx],
					strerror(errno));
				// increment error counter
				founderror++;
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
	if (! readstdin) { free_str_arr(numfiles,filenames); }
	// return appropriate code based on if match was found or any errors
	//  occurred
	// if there are any errors, regardless of if there are any matches,
	//  return with the error code
	if (founderror>0) { return(R_ERROR); }
	// if there were no errors and some lines were matched, return with the
	//  match code. this also includes any lines not matching when the '-v'
	//  or '--invert-match' option is specified
	else if (foundmatch>0) { return(R_MATCH); }
	// if there were no errors and no lines were matched, return with the
	//  no match code. this also includes if there were no non-matching
	//  lines (i.e. all lines matched) when the '-v' or '--invert-match'
	//  option is specified
	else { return(R_NOMATCH); }
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
	// END CLEANUP AND RETURN
	////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// grep_stream function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// fpntr is an open file stream
// string is the search string
// file_pathname is the file path that was open, null if stdin
// invert is boolean to match normally or invert match
// printfname is boolean to print filenames before matching lines if more than
//  one file is given in args
// reads line-by-line through the file, matches lines based on the search
//  string, prints them to stdout, returns number of matched (printed) lines
// note: this function should always return, never calling exit()
int grep_stream(FILE *fpntr, char *string, char *file_pathname, int invert,
	int printfname) {
	// initialize return code to zero
	// increments by 1 if line is matched
	int returncode = 0;
	// string to store each line
	char *line;
	// char to peek ahead
	char peek;
	
	// iteratively call function to get next line from the stream until the
	//  end of the file is reached
	while (feof(fpntr) == 0) {
		// peek ahead one character
		peek = fgetc(fpntr);
		// if peeking didn't cause EOF, put character back
		if (feof(fpntr) == 0) { ungetc(peek,fpntr); }
		// if it did cause EOF, break out of loop, we are done
		// this turns out to be necessary when inverse match option is
		//  specified, otherwise last line of output will be blank line
		//  because get_next_line function will try to read last char of
		//  file, find it is a newline so it will not be added to the
		//  buffer, but the null char will, making an empty string that
		//  will be printed because it doesn't match the search string
		else { break; }
		// get next line from stream
		// note: be aware that this function returns a pointer to a
		//        buffer (char array) that may be larger than the
		//        line. the buffer should contain the line as a null-
		//        terminated string, so string operations should be
		//        safe - however, be aware that there is likely other
		//        data in the buffer beyond the end of the string, so
		//        caution with non-string operations is indicated
		line = get_next_line(fpntr);
		// if there was a problem getting next line, print error and
		//  return with negative error code to indicate problem to
		//  calling function
		if (line == NULL) {
			fprintf(stderr,"%s: error reading line from file '%s': "
				"%s\n",PROG_NAME,file_pathname,strerror(errno));
			return(-1);
		}
		// for each returned line, check if it contains the search
		//  string
		if (strstr(line,string) != NULL) {
			// if the line should be as normal match, print to
			//  stdout
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
				// otherwise, just print non-matching line
				else { printf("%s\n",line); }
			}
		}
		// free allocated memory for this line
		free(line);
	}
	// return number of matched or non-matching lines
	return(returncode);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// get_next_line function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// fpntr is an open file stream
// read the next line from a stream and return it as a string, returns NULL if
//  an I/O error occurs or the end of the file is reached
// note: the newline character is not considered part of the line and is
//        therefore not returned by this function
// note: this function should not print any error messages or other output and
//        it must always return
char *get_next_line(FILE *fpntr) {
	// current size of the buffer
	int buffsize = BUFF_SIZE;
	// char array to store line to return initialized with current buffer
	//  size
	char *line = malloc((buffsize+1) * sizeof(char));
	// return null from the function if malloc fails
	if (line == NULL) { return(NULL); }
	// int to store each char returned from stream
	int currchar;
	// counter for place in buffer
	int count = 0;

	// iteratively call fgetc() to obtain the next character from
	//  stream until the end of the current line is detected, the end of the
	//  file is reached, or error occurs when reading
	// note that fgetc() has an int return type due to the possibility
	//  of errors or EOF (normally -1) - the difference can be
	//  distinguished using ferror() or feof() or even errno
	// this function can handle three types of line ending: "\n", "\r", or
	//  "\r\n"
	while ((currchar = fgetc(fpntr)) != (int) '\n' &&
		currchar != (int) '\r' && feof(fpntr) == 0 &&
		ferror(fpntr) == 0)
	{
		// if the buffer size has been reached, increase the size of the
		//  buffer and then add the char
		if ((count+1) == buffsize) {
			// double the buffer size
			buffsize *= 2;
			// reallocate memory up to the new buffer size
			char *temp = realloc(line, (buffsize+1));
			// return null from the function if realloc fails
			if (temp == NULL) { return(NULL); }
			// otherwise, set the buffer pointer to the newly
			//  allocated space
			else { line = temp; }
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
	}
	// if there was an error, return null from function
	if (ferror(fpntr) != 0 ) { return(NULL); }
	// deal with windows line ending, carriage return and line feed (\r\n)
	//  by getting next char but not adding  it to the buffer if it is a
	//  '\n' - if it is not '\n', put it back
	if (currchar == '\r' && (currchar = fgetc(fpntr)) != (int) '\n') {
		ungetc(currchar,fpntr);
	}
	// stringify the buffer by adding null char at the end of the chars
	line[count] = '\0';
	// return next line in the stream as a string
	return(line);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// free_str_arr function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// size is number of elements in string array
// arr is the string array
// frees allocated memory for a string array
void free_str_arr(int size, char **arr) {
	int idx;
	for (idx=0; idx < size; idx++) {
		free(arr[idx]);
	}
	free(arr);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// print_usage function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// progname is the designated name of this program
// prints usage message
void print_usage(char *progname) {
	fprintf(stderr,"Usage: %s [-v|--invert-match] STRING [FILE]...\n",
		progname);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// remove_str function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// arr is array of strings
// index is index to remove
// len is length of the array
// removes string from array at index
void remove_str(char **arr, int index, int len) {
	// free memory of removed element
	free(arr[index]);
	// index to keep track of position in array
	int idx;
	// loop through array starting at given index, moving each element back
	//  by one until the next-from-last element is reached
	for (idx=index; idx<len-1; idx++) { arr[idx] = arr[idx+1]; }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// print_str_arr function
// ONLY FOR DEBUGGING PURPOSES
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// arr is array of strings
// len is length of array
// prints the array of strings, one per line
void print_str_arr(char **arr, int len) {
	// index to keep track of position in array
	int idx;
	// loop through array from beginning to end, printing each element on a
	//  separate line as a string
	for (idx=0; idx<len; idx++) { printf("%s\n",arr[idx]); }
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// print_buffer function
// ONLY FOR DEBUGGING PURPOSES
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// buff is buffer array of chars
// len is length of array
// prints the contents of buffer, character and hex, one per line
void print_buffer(char *buff, int len) {
	// index to keep track of position in array
	int idx;
	// loop through array from beginning to end, printing each element on a
	//  separate line showing the character and hex equivalent
	for (idx=0; idx<len; idx++) { printf("buffer[%d]='%c' hex:%x\n",idx,
		buff[idx],buff[idx]); }
}
