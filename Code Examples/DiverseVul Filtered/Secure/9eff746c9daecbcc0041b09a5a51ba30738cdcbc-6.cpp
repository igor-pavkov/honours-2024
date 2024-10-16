static int _pgsql_escape_str(char *to, const char *from)
{
    return PQescapeString(to, from, strlen(from));
}