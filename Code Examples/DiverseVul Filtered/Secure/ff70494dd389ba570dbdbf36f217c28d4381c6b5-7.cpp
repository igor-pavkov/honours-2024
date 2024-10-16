struct oui * load_oui_file(void) {
	FILE *fp;
	char * manuf;
	char buffer[BUFSIZ];
	unsigned char a[2];
	unsigned char b[2];
	unsigned char c[2];
	struct oui *oui_ptr = NULL, *oui_head = NULL;

	if (!(fp = fopen(OUI_PATH0, "r"))) {
		if (!(fp = fopen(OUI_PATH1, "r"))) {
			if (!(fp = fopen(OUI_PATH2, "r"))) {
				if (!(fp = fopen(OUI_PATH3, "r"))) {
					return NULL;
				}
			}
		}
	}

	memset(buffer, 0x00, sizeof(buffer));
	while (fgets(buffer, sizeof(buffer), fp) != NULL) {
		if (!(strstr(buffer, "(hex)")))
			continue;

		memset(a, 0x00, sizeof(a));
		memset(b, 0x00, sizeof(b));
		memset(c, 0x00, sizeof(c));
		// Remove leading/trailing whitespaces.
		trim(buffer);
		if (sscanf(buffer, "%2c-%2c-%2c", a, b, c) == 3) {
			if (oui_ptr == NULL) {
				if (!(oui_ptr = (struct oui *)malloc(sizeof(struct oui)))) {
					fclose(fp);
					perror("malloc failed");
					return NULL;
				}
			} else {
				if (!(oui_ptr->next = (struct oui *)malloc(sizeof(struct oui)))) {
					fclose(fp);
					perror("malloc failed");
					return NULL;
				}
				oui_ptr = oui_ptr->next;
			}
			memset(oui_ptr->id, 0x00, sizeof(oui_ptr->id));
			memset(oui_ptr->manuf, 0x00, sizeof(oui_ptr->manuf));
			snprintf(oui_ptr->id, sizeof(oui_ptr->id), "%c%c:%c%c:%c%c", a[0], a[1], b[0], b[1], c[0], c[1]);
			manuf = get_manufacturer_from_string(buffer);
			if (manuf != NULL) {
				snprintf(oui_ptr->manuf, sizeof(oui_ptr->manuf), "%s", manuf);
				free(manuf);
			} else {
				snprintf(oui_ptr->manuf, sizeof(oui_ptr->manuf), "Unknown");
			}
			if (oui_head == NULL)
				oui_head = oui_ptr;
			oui_ptr->next = NULL;
		}
	}

	fclose(fp);
	return oui_head;
}