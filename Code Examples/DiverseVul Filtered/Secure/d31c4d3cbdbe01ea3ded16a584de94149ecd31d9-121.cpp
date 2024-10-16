static void filterStrings (RBin *bin, RList *strings) {
	RBinString *ptr;
	RListIter *iter;
	r_list_foreach (strings, iter, ptr) {
		char *dec = (char *)r_base64_decode_dyn (ptr->string, -1);
		if (dec) {
			char *s = ptr->string;
			do {
				char *dec2 = (char *)r_base64_decode_dyn (s, -1);
				if (!dec2) {
					break;
				}
				if (!r_str_is_printable (dec2)) {
					free (dec2);
					break;
				}
				free (dec);
				s = dec = dec2;
			} while (true);
			if (r_str_is_printable (dec) && strlen (dec) > 3) {
				free (ptr->string);
				ptr->string = dec;
				ptr->type = R_STRING_TYPE_BASE64;
			} else {
				free (dec);
			}
		}
	}
}