GF_Err Media_SetDrefURL(GF_DataEntryURLBox *dref_entry, const char *origName, const char *finalName)
{
	//for now we only support dref created in same folder for relative URLs
	if (strstr(origName, "://") || ((origName[1]==':') && (origName[2]=='\\'))
		|| (origName[0]=='/') || (origName[0]=='\\')
	) {
		dref_entry->location = gf_strdup(origName);
	} else {
		char *fname = strrchr(origName, '/');
		if (!fname) fname = strrchr(origName, '\\');
		if (fname) fname++;

		if (!fname) {
			dref_entry->location = gf_strdup(origName);
		} else {
			u32 len = (u32) (fname - origName);
			if (!finalName || strncmp(origName, finalName, len)) {
				GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("Concatenation of relative path %s with relative path %s not supported, use absolute URLs\n", origName, finalName));
				return GF_NOT_SUPPORTED;
			} else {
				dref_entry->location = gf_strdup(fname);
			}
		}
	}
	return GF_OK;
}