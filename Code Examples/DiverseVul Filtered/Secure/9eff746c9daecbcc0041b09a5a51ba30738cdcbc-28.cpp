static void _sqlite3_close(void *db)
{
    sqlite3_close((sqlite3*)db);
}