/* Author: Marcus Ross
 *         MR867434
 * Description: This program builds a database and executes queries on it, together according to several input files.
 *   It takes as arguments the filenames of plain text files, one containing a configuration and the other a list of queries.
 *   The query file must have one statement per line, from one of the following:
 *     select tableName columnName comparisonOperator operand
 *     numRows tableName
 *     rowStat tableName
 *     numCols tableName
 *     colStat tableName
 *     project tableName columnName
 *     stop
 *   The configuration file on its first line must have an integer indicating the number of tables in the database, followed by the names of each of the tables, one per line.
 *   The schema for each table must be in a plain text file with the same name as the table but followed by ".table", and that file must have on its first line the number of columns in that table and one column name on each following line. The data for each table must in a binary file with the same name as the table but followed by ".data", and each database row represented in that file must have its fields in the same order as they appear in the schema file.
 */

#include <stdlib.h>
#include <stdio.h>
#include "dbStruct.h"
#include "proto.h"

int main(int argc, char** argv) {
	database* db; /* declare the database that will hold everything */
	
	if (argc != 3) { /* kill program if wrong number of arguments */
		fprintf(stderr, "Required arguments: configfilename queryfilename\n");
		return -1;
	}
	
	db = (database *) malloc(sizeof(database));
	initDatabase(&db, argv[1]); /* initialise database using the config filename */
	processQueries(&db, argv[2]); /* process the queries using the query filename */
	
	return 0;
}