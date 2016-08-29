/* Author: Marcus Ross
 *         MR867434
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "dbStruct.h"
#include "proto.h"

/* This function takes as arguments a fully initialised and populated struct database and the name of a plain text file that contains SQL-like statements, one per line. It reads each line of the query file, processes that query, then repeats until the end of the file is reached. */
void processQueries(database** db, char* queryFileN) {
	/*declarations */
	FILE *queryFile; /* the file named by queryFileN */
	char query[60]; /* where the query will be read to */
	char *tokens[15]; /* for tokenising the query */
	
	queryFile = fopen(queryFileN, "r"); /* open the query file named by the argument */
	
	if (!queryFile) { /* kill program if queryFile is null because it failed to open */
		perror("The query file failed to open");
		exit(-1);
	}
	
	while (fgets(query, 60, queryFile)) { /* read each line of the query file */
		if (query[strlen(query) - 1] == '\n') { /* truncate newline character from the end of the query if it's there */
			query[strlen(query) - 1] = '\0'; 
		}
		
		tokens[0] = strtok(query, " "); /* get the first word of the query */
		
		if (strcmp(tokens[0], "select") == 0) { /* if first word of query is select */
			tokens[1] = strtok(NULL, " "); /* word 2 should be table name */
			tokens[2] = strtok(NULL, " "); /* word 3 is col name */
			tokens[3] = strtok(NULL, " "); /* word 4 is comparison operator */
			tokens[4] = strtok(NULL, " "); /* word 5 is a value */
			selectQ(db, tokens[1], tokens[2], tokens[3], tokens[4]); /* run the select query */
		}
		else {
			if (strcmp(tokens[0], "numRows") == 0) { /* if first word of query is numRows */
				tokens[1] = strtok(NULL, " "); /* word 2 is a table name */
				numRows(db, tokens[1]); /* run the numRows query */
			}
			else {
				if (strcmp(tokens[0], "rowStat") == 0) { /* if first word of query is rowStat */
					tokens[1] = strtok(NULL, " "); /* word 2 is a table name */
					rowStat(db, tokens[1]); /* run the rowStat query */
				}
				else {
					if (strcmp(tokens[0], "numCols") == 0) { /* if first word of query is numCols */
						tokens[1] = strtok(NULL, " "); /* word 2 is a table name */
						numCols(db, tokens[1]); /* run the numCols query */
					}
					else {
						if (strcmp(tokens[0], "colStat") == 0) { /* if first word of query is colStat */
							tokens[1] = strtok(NULL, " "); /* word 2 is a table name */
							tokens[2] = strtok(NULL, " "); /* word 3 is a col name */
							colStat(db, tokens[1], tokens[2]); /* run the colStat query */
						}
						else {
							if (strcmp(tokens[0], "project") == 0) { /* if first word of query is project */
								tokens[1] = strtok(NULL, " "); /* word 2 is a table name */
								tokens[2] = strtok(NULL, " "); /* word 3 is a col name */
								project(db, tokens[1], tokens[2]); /* run the project query */
							}
							else {
								if (strcmp(tokens[0], "stop") == 0) { /* if the first word of query is stop */
									break; /* then stop reading lines from query file */
								}
								else { /* if the first word of the query was not any of the above, throw error */
									fprintf(stderr, "Invalid query: \"%s\"\n\n", tokens[0]);
								}
							}
						}
					}
				}
			}
		}
		
	}
	
	fclose(queryFile);
}

/* This function takes as arguments a fully initialised and populated struct database, a table name, a column name, a string containing one operator of ==, !=, >=, >, <=, or <, and a string containing either a word or an int as text. It prints to stdout each row from that table, if it exists, that matches according to the column, operator, and value. */
void selectQ(database** db, char* table, char* column, char* cmp, char* value) {
	int i, j, k; /* for loop counters */
	row **rows; /* this array will hold all the rows that match the select criteria */
	int numRows = 0; /* number of rows held by the array */
	int number; /* for holding the argument "value" after converting to an int */
	int length; /* used when taking the quotation marks off the last word in a select statement */
	
	for (i = 0; i < (*db)->numTables; i++) { /* for each table */
		if (strcmp((*db)->tables[i]->name, table) == 0) { /* if the table matches the table being selected */
			for (j = 0; j < (*db)->tables[i]->numCols; j++) { /* for each column in that table */
				if (strcmp((*db)->tables[i]->cols[j]->name, column) == 0) { /* if the column name matches the column being selected */
					rows = (row **) malloc((*db)->tables[i]->numRows * sizeof(row *)); /* then the query is valid, so alloc the results array */
					
					if (strcmp((*db)->tables[i]->cols[j]->type, "str") == 0) { /* if the column type is string */
						if (value[0] != '"') { /* then if the value doesn't begin with a quotation mark, throw error and stop query */ 
							printf("select failed: cannot compare strings and integers\n\n");
							return;
						}
						
						length = strlen(value); /* get the length of the comparison value */
						value[length - 1] = '\0'; /* truncate the last character to remove the quotation mark */
						value++; /* truncate the first character to remove the quotation mark */
						
						if (strcmp(cmp, "==") == 0) { /* if the operator is == */
							for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
								if (strcmp((*db)->tables[i]->rows[k]->fields[j]->string, value) == 0) { /* if the row's jth field matches the value */
									rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
									rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row then increment numRows */
								}
							}
						}
						else { /* if the operator isn't equals */
							if (strcmp(cmp, "!=") == 0) { /* if operator is != */
								for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
									if (strcmp((*db)->tables[i]->rows[k]->fields[j]->string, value) != 0) { /* if the row's jth field doesn't match the value */
										rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
										rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row then increment numRows */
									}
								}
							}
							else { /* if operator isn't == or !=, then it's bad */
								printf("select failed: %s operator inappropriate for type string\n\n", cmp);
								return;
							}
						}
					}
					else { /* if the column type is integer */
						if (value[0] == '"') { /* if the operand to compare with begins with a quotation mark, throw error and stop query */
							printf("select failed: cannot compare strings and integers\n\n");
							return;
						}
						
						number = atoi(value); /* parse an int from the string */
						
						if (strcmp(cmp, "!=") == 0) { /* if the operator is != */
							for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
								if ((*db)->tables[i]->rows[k]->fields[j]->number != number) { /* if the row's jth field doesn't match the value */
									rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
									rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row */
								}
							}
						}
						else {
							if (strcmp(cmp, "==") == 0) { /* if the operator is == */
								for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
									if ((*db)->tables[i]->rows[k]->fields[j]->number == number) { /* if the row's jth field equals the value */
										rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
										rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row */
									}
								}
							}
							else {
								if (strcmp(cmp, "<=") == 0) { /* if the operator is <= */
									for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
										if ((*db)->tables[i]->rows[k]->fields[j]->number <= number) { /* if the row's jth field is <= the value */
											rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
											rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row */
										}
									}
								}
								else {
									if (strcmp(cmp, "<") == 0) { /* if the operator is < */
										for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
											if ((*db)->tables[i]->rows[k]->fields[j]->number < number) { /* if the row's jth field is < the value */
												rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
												rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row */
											}
										}
									}
									else {
										if (strcmp(cmp, ">=") == 0) { /* if the operator is >= */
											for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
												if ((*db)->tables[i]->rows[k]->fields[j]->number >= number) { /* if the row's jth field is >= the value */
													rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
													rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row */
												}
											}
										}
										else { /* if the operator is > */
											for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row in the table */
												if ((*db)->tables[i]->rows[k]->fields[j]->number > number) { /* if the row's jth field is > the value */
													rows[numRows] = (row *) malloc(sizeof(row)); /* alloc a row in the results array */
													rows[numRows++]->fields = (*db)->tables[i]->rows[k]->fields; /* set the field pointers in the row to the field pointers in the database's row */
												}
											}
										}
									}
								}
							}
						}
					}
					
					if (numRows == 0) { /* numRows will be zero if no rows were added to the results array, then say the query failed */
						printf("select failed: no matches found %s %s in %s column in %s table\n\n", cmp, value, column, table);
						return;
					}
					
					printf("Table: %s\nselect: %s %s %s\n", table, column, cmp, value); /* output heading */
					
					for (k = 0; k < numRows; k++) { /* for each row in the results array */
						for (j = 0; j < (*db)->tables[i]->numCols; j++) { /* for each column in the results array */
							if (strcmp((*db)->tables[i]->cols[j]->type, "str") == 0) { /* if that column is a string, output a string */
								printf("%-*s", (*db)->tables[i]->cols[j]->size, rows[k]->fields[j]->string);
							}
							else { /* otherwise, that column is an int, so output an int */
								printf("%4i ", rows[k]->fields[j]->number);
							}
						}
						
						printf("\n");
					}
					
					printf("\n");
					free(rows); /* results array not needed */
					return;
				}
			}
			
			printf("select failed: %s column was not found\n\n", column); /* reached only when no column names match the query */
			return;
		}
	}
	
	printf("select failed: %s table was not found\n\n", table); /* reached only when no table names match the query */
}

/* This function takes as arguments a fully initialised and populated struct database and a table name. It prints to stdout the number of rows in that table, if it exists. */
void numRows(database** db, char* table) {
	int i; /* for loop counter */
	
	for (i = 0; i < (*db)->numTables; i++) { /* for each table in the databse */
		if (strcmp((*db)->tables[i]->name, table) == 0) { /* if the table name matches the argument */
			printf("Table: %s\nnumRows: %i\n\n", table, (*db)->tables[i]->numRows); /* output the table's number of rows */
			return;
		}
	}
	
	printf("numRow failed: %s table was not found\n\n", table); /* query failed if no table names matched */
}

/* This function takes as arguments a fully initialised and populated struct database and a table name. It prints to stdout the size of that table's rows, if it exists. */
void rowStat(database** db, char* table) {
	int i; /* for loop counter */
	
	for (i = 0; i < (*db)->numTables; i++) { /* for each table in the database */
		if (strcmp((*db)->tables[i]->name, table) == 0) { /* if the table name matches the argument */
			printf("Table: %s\nrowStat: %i byte%s\n\n", table, (*db)->tables[i]->rowSize, (*db)->tables[i]->rowSize > 1 ? "s" : ""); /* output the size of that table's rows */
			return;
		}
	}
	
	printf("rowStat failed: %s table was not found\n\n", table); /* query failed if no table names matched */
}

/* This function takes as arguments a fully initialised and populated struct database and a table name. It prints to stdout the number of columns in that table, if it exists. */
void numCols(database** db, char* table) {
	int i; /* for loop counter */
	
	for (i = 0; i < (*db)->numTables; i++) { /* for each table */
		if (strcmp((*db)->tables[i]->name, table) == 0) { /* if the table name matches the argument */
			printf("Table: %s\nnumCols: %i\n\n", table, (*db)->tables[i]->numCols); /* output that table's number of columns */
			return;
		}
	}
	
	printf("numCols failed: %s table was not found\n\n", table); /* query failed if no table names matched */
}

/* This function takes as arguments a fully initialised and populated struct database, a table name, and a column name. It prints to stdout the size of that column of that table, if they exist. */
void colStat(database** db, char* table, char* column) {
	int i, j; /* for loop counters */
	
	for (i = 0; i < (*db)->numTables; i++) { /* for each table */
		if (strcmp((*db)->tables[i]->name, table) == 0) { /* if that table's name matches the argument */
			for (j = 0; j < (*db)->tables[i]->numCols; j++) { /* for each column */
				if (strcmp((*db)->tables[i]->cols[j]->name, column) == 0) { /* if that table's name matches the argument */
					printf("Table: %s\nColumn: %s\ncolStat: %i byte %s\n\n", table, column, (*db)->tables[i]->cols[j]->size, strcmp((*db)->tables[i]->cols[j]->type, "str") == 0 ? "string" : "integer"); /* output that column's size */
					return;
				}
			}
			
			printf("colStat failed: %s column was not found\n\n", column); /* query failed if no matching column found */
			return;
		}
	}
	
	printf("colStat failed: %s table was not found\n\n", table); /* query failed if no matching table found */
}

/* This function takes as arguments a fully initialised and populated struct database, a table name, and a column name. It prints to stdout each distinct value found in that column of that table, if they exist. */
void project(database** db, char* table, char* column) {
	int i, j, k, l; /* for loop counters */
	field **fields; /* array to hold the results that match the criteria */
	int numFields = 0; /* the number of fields held in the results array */
	bool duplicate = false; /* for knowing whether a field value has been encountered already when adding to the results array */
	
	for (i = 0; i < (*db)->numTables; i++) { /* for each table */
		if (strcmp((*db)->tables[i]->name, table) == 0) { /* if it's the right table */
			for (j = 0; j < (*db)->tables[i]->numCols; j++) { /* for each column in that table */
				if (strcmp((*db)->tables[i]->cols[j]->name, column) == 0) { /* if this is the right column */
					fields = (field **) malloc((*db)->tables[i]->numRows * sizeof(field *)); /* alloc for the results array */
					
					if (strcmp((*db)->tables[i]->cols[j]->type, "str") == 0) { /* if the column is of type string */
						for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row */
							duplicate = false; /* assume it's not in the results array already */
							
							for (l = 0; l < numFields; l++) { /* for each field in the results array */
								if (strcmp((*db)->tables[i]->rows[k]->fields[j]->string, fields[l]->string) == 0) {
									duplicate = true; /* check if the row is in the results already */
									break; /* stop traversing the results array if it is */
								}
							}
							
							if (!duplicate) { /* if it's not already in the results array */
								fields[numFields] = (field *) malloc(sizeof(field)); /* alloc for a slot in the results array */
								fields[numFields++]->string = (*db)->tables[i]->rows[k]->fields[j]->string; /* then copy data to the slot and increment numFields */
							}
						}
						
						printf("Table: %s\nColumn: %s\nproject:\n", (*db)->tables[i]->name, (*db)->tables[i]->cols[j]->name); /* output heading */
						
						for (l = 0; l < numFields; l++) { /* print each field in the results array */
							printf("\t%s\n", fields[l]->string);
						}
						
						printf("\n");
					}
					else { /* column is of type integer */
						for (k = 0; k < (*db)->tables[i]->numRows; k++) { /* for each row */
							duplicate = false; /* assume it's not a duplicate */
							
							for (l = 0; l < numFields; l++) { /* for each field in the results array */
								if ((*db)->tables[i]->rows[k]->fields[j]->number == fields[l]->number) {
									duplicate = true; /* check if the row is in the results already */
									break; /* stop traversing the results array if it is */
								}
							}
							
							if (!duplicate) { /* if it wasn't in the results already */
								fields[numFields] = (field *) malloc(sizeof(field)); /* alloc for a slot in the results array */
								fields[numFields++]->number = (*db)->tables[i]->rows[k]->fields[j]->number; /* then copy data to the slot and increment numFields */
							}
						}
						
						printf("Table: %s\nColumn: %s\nproject:\n", (*db)->tables[i]->name, (*db)->tables[i]->cols[j]->name); /* output heading */
						
						for (l = 0; l < numFields; l++) { /* print each field in the results array */
							printf("\t%i\n", fields[l]->number);
						}
						
						printf("\n");
					}
					
					free(fields);
					return;
				}
			}
			
			printf("project failed: %s column was not found\n\n", column);
			return; /* reached only when no column names match the query */
		}
	}
	
	printf("project failed: %s table was not found\n\n", table); /* reached only when no table names match the query */
}