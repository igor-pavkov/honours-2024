static int _mysql_escape_str(char *to, const char *from)
{
    return mysql_escape_string(to, from, strlen(from));
}