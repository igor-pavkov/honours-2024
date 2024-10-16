GF_FileType get_file_type_by_ext(char *inName)
{
	GF_FileType type = GF_FILE_TYPE_NOT_SUPPORTED;
	char *ext = strrchr(inName, '.');
	if (ext) {
		char *sep;
		if (!strcmp(ext, ".gz")) ext = strrchr(ext-1, '.');
		ext+=1;
		sep = strchr(ext, '.');
		if (sep) sep[0] = 0;

		if (!stricmp(ext, "mp4") || !stricmp(ext, "3gp") || !stricmp(ext, "mov") || !stricmp(ext, "3g2") || !stricmp(ext, "3gs")) {
			type = GF_FILE_TYPE_ISO_MEDIA;
		} else if (!stricmp(ext, "bt") || !stricmp(ext, "wrl") || !stricmp(ext, "x3dv")) {
			type = GF_FILE_TYPE_BT_WRL_X3DV;
		} else if (!stricmp(ext, "xmt") || !stricmp(ext, "x3d")) {
			type = GF_FILE_TYPE_XMT_X3D;
		} else if (!stricmp(ext, "lsr") || !stricmp(ext, "saf")) {
			type = GF_FILE_TYPE_LSR_SAF;
		} else if (!stricmp(ext, "svg") || !stricmp(ext, "xsr") || !stricmp(ext, "xml")) {
			type = GF_FILE_TYPE_SVG;
		} else if (!stricmp(ext, "swf")) {
			type = GF_FILE_TYPE_SWF;
		} else if (!stricmp(ext, "jp2")) {
			if (sep) sep[0] = '.';
			return GF_FILE_TYPE_NOT_SUPPORTED;
		}
		else type = GF_FILE_TYPE_NOT_SUPPORTED;

		if (sep) sep[0] = '.';
	}


	/*try open file in read mode*/
	if (!type && gf_isom_probe_file(inName)) type = GF_FILE_TYPE_ISO_MEDIA;
	return type;
}