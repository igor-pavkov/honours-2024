static char * exif_get_tagname(int tag_num, char *ret, int len, tag_table_type tag_table)
{
	int i, t;
	char tmp[32];

	for (i = 0; (t = tag_table[i].Tag) != TAG_END_OF_LIST; i++) {
		if (t == tag_num) {
			if (ret && len)  {
				strlcpy(ret, tag_table[i].Desc, abs(len));
				if (len < 0) {
					memset(ret + strlen(ret), ' ', -len - strlen(ret) - 1);
					ret[-len - 1] = '\0';
				}
				return ret;
			}
			return tag_table[i].Desc;
		}
	}

	if (ret && len) {
		snprintf(tmp, sizeof(tmp), "UndefinedTag:0x%04X", tag_num);
		strlcpy(ret, tmp, abs(len));
		if (len < 0) {
			memset(ret + strlen(ret), ' ', -len - strlen(ret) - 1);
			ret[-len - 1] = '\0';
		}
		return ret;
	}
	return "";
}