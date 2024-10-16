static char *exif_get_sectionlist(int sectionlist)
{
	int i, len, ml = 0;
	char *sections;

	for(i=0; i<SECTION_COUNT; i++) {
		ml += strlen(exif_get_sectionname(i))+2;
	}
	sections = safe_emalloc(ml, 1, 1);
	sections[0] = '\0';
	len = 0;
	for(i=0; i<SECTION_COUNT; i++) {
		if (sectionlist&(1<<i)) {
			snprintf(sections+len, ml-len, "%s, ", exif_get_sectionname(i));
			len = strlen(sections);
		}
	}
	if (len>2)
		sections[len-2] = '\0';
	return sections;
}