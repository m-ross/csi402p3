/* Author: Marcus Ross
 *         MR867434
 */

typedef union field { /* the field contains either a string or an int */
	char *string;
	int number;
} field;

typedef struct row {
	field **fields; /* array of pointers to fields; one field for each column in the table */
} row;

typedef struct col {
	char *name; /* the name of the column */
	char *type; /* the type of the column, either "int" or "str" */
	int size; /* the sizeof the column in bytes */
} col;

typedef struct table {
	char *name; /* the name of the table */
	row **rows; /* array of pointers to rows */
	col **cols; /* array of pointers to columns */
	int numRows, numCols, rowSize; /* the quantities of rows and columns, and the size of each row */
} table;

typedef struct database { /* only one database exists per execution */
	int numTables; /* the quantity of tables, gleaned from the config file */
	table **tables; /* array of pointers to tables */
} database;