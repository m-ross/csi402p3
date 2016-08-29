/* Author: Marcus Ross
 *         MR867434
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "const.h"
#include "dbStruct.h"

void initDatabase(database** db, char* confFileN) {
	/* declarations */
	FILE *confFile, *tableFile, *tableDataFile;
	int i, j, k; /* for loop counters */
	char *tableFileName, *tableDataFileName; /* for holding the constructed filenames */
	char colName[MAX_LEN_NAME], colType[3]; /* for holding column names and types */
	
	confFile = fopen(confFileN, "r"); /* open the config file named by argument 2 */
	
	if (!confFile) { /* kill program if confFile is null because it failed to open */
		perror("The configuration file failed to open");
		exit(-1);
	}
	
	if (fscanf(confFile, "%d", &(*db)->numTables) != 1 || (*db)->numTables > MAX_TABLES) { /* kill program if fscanf failed to read an int to numTables or if the number read is too great */
		fprintf(stderr, "The configuration file is not properly formatted.\n");
		exit(-1);
	}
	
	(*db)->tables = (table **) malloc((*db)->numTables * sizeof(table *)); /* allocate space for a number of tables equal to numTables */
	
	for (i = 0; i < (*db)->numTables; i++) { /* read each remaining line of confFile as a table name */
		(*db)->tables[i] = (table *) malloc(sizeof(table));
		(*db)->tables[i]->name = (char *) malloc(sizeof(char) * MAX_LEN_NAME); /* allocate space for the table's name */
	
		if (fscanf(confFile, "%s", (*db)->tables[i]->name) != 1) { /* kill program if failed to read a name */
			fprintf(stderr, "The configuration file is not properly formatted.\n");
			exit(-1);
		}
		
		tableFileName = (char *) malloc(strlen((*db)->tables[i]->name) + 7); /* allocate space for a filename */
		tableDataFileName = (char *) malloc(strlen((*db)->tables[i]->name) + 6); /* allocate space for a filename */
		strcpy(tableFileName, (*db)->tables[i]->name); /* copy name of table to schema filename */
		strcpy(tableDataFileName, (*db)->tables[i]->name); /* copy name of table to data filename */
		strcat(tableFileName, ".table\0"); /* concatenate this extension onto the table name */
		strcat(tableDataFileName, ".data\0"); /* concatenate this extension onto the table name */
		tableFile = fopen(tableFileName, "r"); /* open schema file */
		
		if (!tableFile) { /* if the file failed to open, kill program */
			perror("The table schema file failed to open");
			exit(-1);
		}
		
		tableDataFile = fopen(tableDataFileName, "r"); /* open data file */
		
		if (!tableDataFile) { /* if the file failed to open, kill program */
			perror("The table data file failed to open");
			exit(-1);
		}
		
		free(tableFileName); /* filenames not needed anymore */
		free(tableDataFileName);
		
		if (fscanf(tableFile, "%d", &((*db)->tables[i]->numCols)) != 1 || (*db)->tables[i]->numCols > MAX_COLS) {
			fprintf(stderr, "The table file is not properly formatted.\n");
			exit(-1); /* kill program if failed to scan a number into numCols or if the number read was too great */
		}
		
		(*db)->tables[i]->rowSize = 0; /* initialise it so it can be incremented */
		(*db)->tables[i]->cols = (col **) malloc((*db)->tables[i]->numCols * sizeof(col *)); /* alloc space for cols = numCols * sizeof(col) */
		
		for (j = 0; j < (*db)->tables[i]->numCols; j++) { /* do for each expected column */
			(*db)->tables[i]->cols[j] = (col *) malloc(sizeof(col));
			fscanf(tableFile, "%s\t%s\t%d", colName, colType, &(*db)->tables[i]->cols[j]->size); /* read a line */
			(*db)->tables[i]->cols[j]->name = (char *) malloc(strlen(colName)); /* alloc space for the name */
			(*db)->tables[i]->cols[j]->type = (char *) malloc(strlen(colType)); /* alloc space for the type */
			strcpy((*db)->tables[i]->cols[j]->name, colName); /* copy in the name */
			strcpy((*db)->tables[i]->cols[j]->type, colType); /* copy in the type */
			(*db)->tables[i]->rowSize += (*db)->tables[i]->cols[j]->size; /* size of each row is the sum of the column sizes */
		}
		
		fseek(tableFile, 0, SEEK_SET); /* rewind the schema file */
		fseek(tableDataFile, 0, SEEK_END); /* goto end of the data file */
		(*db)->tables[i]->numRows = ftell(tableDataFile) / (*db)->tables[i]->rowSize; /* the number of rows = the size of the file divided by the size of each row */
		fseek(tableDataFile, 0, SEEK_SET); /* rewind the data file */
		(*db)->tables[i]->rows = (row **) malloc((*db)->tables[i]->numRows * sizeof(row *)); /* alloc for rows = numRows * sizeof(row) */
		
		for (j = 0; j < (*db)->tables[i]->numRows; j++) { /* do once for each row/each line of the data file */
			(*db)->tables[i]->rows[j] = (row *) malloc(sizeof(row));
			(*db)->tables[i]->rows[j]->fields = (field **) malloc((*db)->tables[i]->numCols * sizeof(field *)); /* alloc for the fields in each row */
			
			for (k = 0; k < (*db)->tables[i]->numCols; k++) { /* do once for each column/each value on each line */
				(*db)->tables[i]->rows[j]->fields[k] = (field *) malloc(sizeof(field));
				if (strcmp("str", (*db)->tables[i]->cols[k]->type) == 0) { /* if the value about to be read is expected to be a string, then store it as a string */
					(*db)->tables[i]->rows[j]->fields[k]->string = (char *) malloc(sizeof(char) * (*db)->tables[i]->cols[k]->size);
					fread((*db)->tables[i]->rows[j]->fields[k]->string, (*db)->tables[i]->cols[k]->size, 1, tableDataFile);
				}
				else { /* if it's not a string, store it as a number */
					fread(&(*db)->tables[i]->rows[j]->fields[k]->number, (*db)->tables[i]->cols[k]->size, 1, tableDataFile);
				}
			}
		}
		
		fclose(tableFile);
		fclose(tableDataFile);
	}
	
	fclose(confFile);
}