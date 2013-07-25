
#ifndef TEXTFILE_H_
#define TEXTFILE_H_

#include <stdio.h>

size_t GetDelim(char** linebuff, size_t* linebufsz, int delim, FILE* fd);
size_t GetLine(char** linebuff, size_t *n, FILE *fd);
char* ReadTextFile(char* filename);

#endif /* TEXTFILE_H_ */
