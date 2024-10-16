char * sanitize_xml(unsigned char * text, int length)
{
	int i;
	size_t len;
	unsigned char * pos;
	char * newpos;
	char * newtext = NULL;
	if (text != NULL && length > 0) {
		len = 6 * length;
		newtext = (char *)calloc(1, (len + 1) * sizeof(char)); // Make sure we have enough space
		pos = text;
		for (i = 0; i < length; ++i, ++pos) {
			switch (*pos) {
				case '&':
					strncat(newtext, "&amp;", len);
					break;
				case '<':
					strncat(newtext, "&lt;", len);
					break;
				case '>':
					strncat(newtext, "&gt;", len);
					break;
				case '\'':
					strncat(newtext, "&apos;", len);
					break;
				case '"':
					strncat(newtext, "&quot;", len);
					break;
				default:
					if ( isprint((int)(*pos)) || (*pos)>127 ) {
						newtext[strlen(newtext)] = *pos;
					} else {
						newtext[strlen(newtext)] = '\\';
						newpos = newtext + strlen(newtext);
						snprintf(newpos, strlen(newpos) + 1, "%3u", *pos);
					}
					break;
			}
		}
		newtext = (char *) realloc(newtext, strlen(newtext) + 1);
	}

	return newtext;
}