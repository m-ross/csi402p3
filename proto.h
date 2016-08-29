/* Author: Marcus Ross
 *         MR867434
 */

void initDatabase(database**, char*);
void processQueries(database**, char*);
void selectQ(database**, char*, char*, char*, char*);
void numRows(database**, char*);
void rowStat(database**, char*);
void numCols(database**, char*);
void colStat(database**, char*, char*);
void project(database**, char*, char*);