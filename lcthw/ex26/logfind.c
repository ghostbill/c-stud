//A tool like grep

#include "dbg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

const size_t MAX_LINE = 1024;

int list_files(glob_t *pglob)
{
	char *line = calloc(MAX_LINE, 1);
	FILE *file = fopen(".logfind", "r");
	int glob_flags = GLOB_TILDE;
	int i = 0;
	int rc = -1;
	
	check(pglob != NULL, "Invalid glob_t given.");
	check_mem(line);
	check(file, "Failed to open .logfind. Create that file first.");
	
	 while(fgets(line, MAX_LINE-1, file) != NULL) {
        line[strlen(line) - 1] = '\0'; // drop the \n ending
        debug("Globbing %s", line);

        rc = glob(line, glob_flags, NULL, pglob);
        check(rc == 0 || rc == GLOB_NOMATCH, "Failed to glob.");

        // dumb work around to a stupid design in glob
        if(glob_flags == GLOB_TILDE) glob_flags |= GLOB_APPEND;
    }

	for(i = 0; i < pglob->gl_pathc; i++) {
		debug("Matched file: %s", pglob->gl_pathv[i]);	
	}
	
	rc = 0; // all good

error: // fallback
	if(line) free(line);
	return rc;
}


int found_it(int use_or, int found_count, int search_len)
{
	debug("use_or: %d, found_count: %d, search_len: %d", use_or, found_count, search_len);
	
	if(use_or && found_count > 0) {
		return 1;
	} else if(!use_or && found_count == search_len) {
		return 1;	
	} else {
		return 0;	
	}

}


int scan_file(const char *filename, int search_len, char *search_for[])
{

	char *line = calloc(MAX_LINE, 1);
	FILE *file = fopen(filename, "r");
	char *found = NULL;
	int i = 0;

	check_mem(line);
	check(file, "Failed to open file: %s", filename);

	//read each line of the file and search that line for the contents
	while(fgets(line, MAX_LINE-1, file) != NULL && found == NULL) {
		for(i = 0; i < search_len && found == NULL; i++) {
			found = strcasestr(line, search_for[i]);
			if(found) {
				printf("%s\n", filename);
			}		
		}
	}
	
	free(line);
	fclose(file);
	return 0;

error:
	if(line) free(line);
	if(file) fclose(file);

	return -1;
}






int main(int argc, char *argv[]) 
{
	check(argc > 2, "USAGE: logfind word word word");
	
	scan_file("logfind.c", argc, argv);

	return 0;

error:
	return 1;
}
