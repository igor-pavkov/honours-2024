static const char *valid_domain_label(const char *label)
{
	unsigned char ch;
	unsigned pos = 0;

	for (;;) {
		ch = *label;
		if ((ch|0x20) < 'a' || (ch|0x20) > 'z') {
			if (ch < '0' || ch > '9') {
				if (ch == '\0' || ch == '.')
					return label;
				/* DNS allows only '-', but we are more permissive */
				if (ch != '-' && ch != '_')
					return NULL;
			}
		}
		label++;
		pos++;
		//Do we want this?
		//if (pos > 63) /* NS_MAXLABEL; labels must be 63 chars or less */
		//	return NULL;
	}
}