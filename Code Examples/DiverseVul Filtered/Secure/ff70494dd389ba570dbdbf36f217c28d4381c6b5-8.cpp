char * get_manufacturer_from_string(char * buffer) {
	char * manuf = NULL;
	char * buffer_manuf;
	if (buffer != NULL && strlen(buffer) > 0) {
		buffer_manuf = strstr(buffer, "(hex)");
		if (buffer_manuf != NULL) {
			buffer_manuf += 6; // skip '(hex)' and one more character (there's at least one 'space' character after that string)
			while (*buffer_manuf == '\t' || *buffer_manuf == ' ') {
				++buffer_manuf;
			}

			// Did we stop at the manufacturer
			if (*buffer_manuf != '\0') {

				// First make sure there's no end of line
				if (buffer_manuf[strlen(buffer_manuf) - 1] == '\n' || buffer_manuf[strlen(buffer_manuf) - 1] == '\r') {
					buffer_manuf[strlen(buffer_manuf) - 1] = '\0';
					if (*buffer_manuf != '\0' && (buffer_manuf[strlen(buffer_manuf) - 1] == '\n' || buffer[strlen(buffer_manuf) - 1] == '\r')) {
						buffer_manuf[strlen(buffer_manuf) - 1] = '\0';
					}
				}
				if (*buffer_manuf != '\0') {
					if ((manuf = (char *)malloc((strlen(buffer_manuf) + 1) * sizeof(char))) == NULL) {
						perror("malloc failed");
						return NULL;
					}
					snprintf(manuf, strlen(buffer_manuf) + 1, "%s", buffer_manuf);
				}
			}
		}
	}

	return manuf;
}