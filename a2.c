/* a2.c - template when a text buffer is needed
 * for a program. This provides basic commands and
 * buffer space. Any commands can be added after 
 * the basic commands.
 *
 * (C) 2022 Kurt Theis
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <ctype.h>

#define BUFSIZE 1000000		// buffer space
#define LINESIZE 80			// max line length


// global variables
int pos = 0;		// current position of the end of the buffer
char *buf = NULL;	// text buffer;



/* *********** */
/* Subroutines */
/* *********** */

// display buffer contents
void showbuf() {
	int index=0;
	if (pos == 0) return;
	for (index=0; index<pos; index++) printf("%c",*(buf+index));
	return;
}

// delete the last line of the buffer
void delete() {
	if (pos == 0) return;
	char ch;
	pos--;
	*(buf+pos) = '\0';	// clear last \n
	while (pos >= 0) {
		ch = *(buf+pos);
		if (ch == '\n') break;
		*(buf+pos)='\0';
		pos--;
		if (pos == 0) return;
	}
	pos++;	// keep \n
	return;
}

// save buffer to file
void save(char line[LINESIZE]) {
	if (pos == 0) return;
	char cmd[10],filename[70];
	memset(cmd,0,sizeof(cmd));
	memset(filename,0,sizeof(filename));
	sscanf(line,"%s %s",cmd,filename);
	filename[strlen(filename)] = '\0';
	if (strlen(filename) < 1) {
			printf("Bad filename\nAborted.\n");
			return;
	}
	FILE *fd = fopen(filename,"w");
	if (fd == NULL) {
		printf("Error creating file %s\nAborted.\n",filename);
		return;
	}
	for (int index = 0; index <= pos; index++) 
		fprintf(fd,"%c",*(buf+index));
	fflush(fd);
	fclose(fd);
	printf("Buffer Saved to %s\n",filename);
	return;
}

// load file to buffer
void load(char line[LINESIZE]) {
	char cmd[10],filename[70];
    memset(cmd,0,sizeof(cmd));
    memset(filename,0,sizeof(filename));
    sscanf(line,"%s %s",cmd,filename);
    filename[strlen(filename)] = '\0';
    if (strlen(filename) < 1) {
            printf("Bad filename\nAborted.\n");
            return;
    }
    FILE *fd = fopen(filename,"r");
    if (fd == NULL) {
        printf("Error reading file %s\nAborted.\n",filename);
        return;
    }
	pos = 0; 
	char ch;
	while (1) {
		ch = fgetc(fd);
		if (feof(fd)) break;
		*(buf+pos)=ch;
		pos++;
	}
	fclose(fd);
	printf("File %s loaded\n",filename);
	return;
}

// view file contents
void view(char line[LINESIZE]) {
	char cmd[10],filename[70];
    memset(cmd,0,sizeof(cmd));
    memset(filename,0,sizeof(filename));
    sscanf(line,"%s %s",cmd,filename);
    filename[strlen(filename)] = '\0';
    if (strlen(filename) < 1) {
            printf("Bad filename\nAborted.\n");
            return;
    }
    FILE *fd = fopen(filename,"r");
    if (fd == NULL) {
        printf("Error reading file %s\nAborted.\n",filename);
        return;
    }
	char ch;
	while (1) {
		ch = fgetc(fd);
		if (feof(fd)) break;
		printf("%c",ch);
	}
	fclose(fd);
	printf("\n");
	return;
}

// show files in the current directory
void showfiles() {
	glob_t globlist;
	int i=0;
	// by using *, . and .. will ALWAYS be found, so NOMATCH is an error
	int retval = glob("*", GLOB_PERIOD, NULL, &globlist);
    if (retval == GLOB_NOSPACE || \
		 retval == GLOB_ABORTED || \
		 retval == GLOB_NOMATCH) {
		printf("glob() error: Aborted.\n");
        globfree(&globlist);
		return;
    }
	printf("\nFiles:\n");
	while (globlist.gl_pathv[i]) {
		if (i > 1) 	// don't show . or ..
			printf("%s\n",globlist.gl_pathv[i]);
		i++;
	}
	globfree(&globlist);
	printf("\n");
	return;
}


// show commands
void help() {	// all commands are prefaced with a .
	printf("\n");
	printf("\nCommands:\n");
	printf(".quit 		exit program, no save\n");
	printf(".list		show buffer\n");
	printf(".save [file]	save buffer to file\n");
	printf(".load [file]	load file to buffer\n");
	printf(".view [file]	display contents of file\n");
	printf(".new		clear buffer\n");
	printf(".del		delete last line of buffer\n");
	printf(".stat		show buffer status\n");
	printf(".dir		display files in current directory\n");
	printf(".help		show this listing\n");
	printf("\n");
	return;
}





/* ************ */
/* Main Routine */
/* ************ */

int main() {

	// setup the buffer, exit on error
	buf = (char *)malloc(BUFSIZE*sizeof(char));
	if (buf==NULL) {
		printf("Memory Error (fatal)\n");
		return 1;
	}
	memset(buf,0,BUFSIZE);		// initial clear
	int SAVED = 0;				// file save flag
	char line[BUFSIZE+1];
	int linelen = 0;

	printf("Type .help for a command list\n");

	// main loop
	while (1) {
		memset(line,0,BUFSIZE);
		fgets(line,LINESIZE,stdin);

		// test commands
		if (strncmp(line,".quit",5)==0) {	// exit program
			free(buf);
			if (!SAVED) printf("(buffer not saved on exit)\n");
			printf("Exiting\n");
			return 0;
		}
		if (strncmp(line,".list",5)==0) {	// display buffer
			showbuf();
			continue;
		}
		if (strncmp(line,".help",5)==0) {	// show commands
			help();
			continue;
		}
		if (strncmp(line,".new",4)==0) {	// clear buffer
			memset(buf,0,BUFSIZE);
			pos = 0;
			if (!SAVED) printf("(buffer not saved)\n");
			printf("Buffer Cleared\n");
			SAVED=0;
			continue;
		}
		if (strncmp(line,".del",4)==0) {	// delete the last line entered
			delete();
			continue;
		}
		if (strncmp(line,".save",5)==0) {	// save the buffer to a named file
			save(line);
			SAVED=1;
			continue;
		}
		if (strncmp(line,".load",5)==0) {	// load a file to buffer
			load(line);
			SAVED=1;
			continue;
		}
		if (strncmp(line,".view",5)==0) {	// display file contents
			view(line);
			continue;
		}
		if (strncmp(line,".stat",5)==0) {	// show buffer status
			int linecount=0;
			for (int i=0; i<pos; i++) 		// count new lines
				if (*(buf+i)=='\n') linecount++;
			printf("Buffer Status:\n");
			printf("Buffer Size (bytes):\t%d\n",pos);
			printf("Lines of text:\t\t%d\n",linecount);
			printf("Buffer Saved:\t\t%s",SAVED? "Yes" : "No");
			printf("\n");
			continue;
		}
		if (strncmp(line,".dir",4)==0) {	// show files in current directory
			showfiles();
			continue;
		}

		// done with tests

		linelen = strlen(line);
		
		// save line to buffer
		if (pos < BUFSIZE+81) {
			for (int i=0; i<linelen; i++) *(buf+pos+i) = line[i];
			pos+=linelen;
			SAVED=0;
			continue;
		} else {
			printf("Warning: buffer full\n");
		}

	}

	// will likely never get here
	free(buf);
	return 0;
}


