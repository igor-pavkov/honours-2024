static int _sqlite3_escape_str(char *to, const char *from)
{
    char s;

    while ( (s = *from++) != '\0' ) {
	if (s == '\'' || s == '\\') {
	    *to++ = '\\';
	}
	*to++ = s;
    }
    *to = '\0';

    return 0;
}