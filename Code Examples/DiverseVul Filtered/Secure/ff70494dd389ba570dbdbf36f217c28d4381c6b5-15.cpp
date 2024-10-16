char *get_manufacturer(unsigned char mac0, unsigned char mac1, unsigned char mac2) {
	static char * oui_location = NULL;
	char oui[OUI_STR_SIZE + 1];
	char *manuf;
	//char *buffer_manuf;
	char * manuf_str;
	struct oui *ptr;
	FILE *fp;
	char buffer[BUFSIZ];
	char temp[OUI_STR_SIZE + 1];
	unsigned char a[2];
	unsigned char b[2];
	unsigned char c[2];
	int found = 0;

	if ((manuf = (char *)calloc(1, MANUF_SIZE * sizeof(char))) == NULL) {
		perror("calloc failed");
		return NULL;
	}

	snprintf(oui, sizeof(oui), "%02X:%02X:%02X", mac0, mac1, mac2 );

	if (G.manufList != NULL) {
		// Search in the list
		ptr = G.manufList;
		while (ptr != NULL) {
			found = ! strncasecmp(ptr->id, oui, OUI_STR_SIZE);
			if (found) {
				memcpy(manuf, ptr->manuf, MANUF_SIZE);
				break;
			}
			ptr = ptr->next;
		}
	} else {
		// If the file exist, then query it each time we need to get a manufacturer.
		if (oui_location == NULL) {
			fp = fopen(OUI_PATH0, "r");
			if (fp == NULL) {
				fp = fopen(OUI_PATH1, "r");
				if (fp == NULL) {
				    fp = fopen(OUI_PATH2, "r");
				    if (fp != NULL) {
					oui_location = OUI_PATH2;
				    }
				} else {
				    oui_location = OUI_PATH1;
				}
			} else {
				oui_location = OUI_PATH0;
			}
		} else {
			fp = fopen(oui_location, "r");
		}

		if (fp != NULL) {

			memset(buffer, 0x00, sizeof(buffer));
			while (fgets(buffer, sizeof(buffer), fp) != NULL) {
				if (strstr(buffer, "(hex)") == NULL) {
					continue;
				}

				memset(a, 0x00, sizeof(a));
				memset(b, 0x00, sizeof(b));
				memset(c, 0x00, sizeof(c));
				if (sscanf(buffer, "%2c-%2c-%2c", a, b, c) == 3) {
					snprintf(temp, sizeof(temp), "%c%c:%c%c:%c%c", a[0], a[1], b[0], b[1], c[0], c[1] );
					found = !memcmp(temp, oui, strlen(oui));
					if (found) {
						manuf_str = get_manufacturer_from_string(buffer);
						if (manuf_str != NULL) {
							snprintf(manuf, MANUF_SIZE, "%s", manuf_str);
							free(manuf_str);
						}

						break;
					}
				}
				memset(buffer, 0x00, sizeof(buffer));
			}

			fclose(fp);
		}
	}

	// Not found, use "Unknown".
	if (!found || *manuf == '\0') {
		memcpy(manuf, "Unknown", 7);
		manuf[strlen(manuf)] = '\0';
	}

	manuf = (char *)realloc(manuf, (strlen(manuf) + 1) * sizeof(char));

	return manuf;
}