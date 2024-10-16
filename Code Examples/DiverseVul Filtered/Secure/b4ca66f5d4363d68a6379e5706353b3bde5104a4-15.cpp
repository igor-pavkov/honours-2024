static bool inline consume_str_new(RBuffer *b, ut64 bound, ut32 *len_out, char **str_out) {
	r_return_val_if_fail (str_out, false);
	*str_out = NULL;
	if (len_out) {
		*len_out = 0;
	}

	ut32 len = 0;
	// module_str
	if (consume_u32_r (b, bound, &len)) {
		char *str = (char *)malloc (len + 1);
		if (str && consume_str_r (b, bound, len, str)) {
			if (len_out) {
				*len_out = len;
			}
			*str_out = str;
			return true;
		}
		free (str);
	}
	return false;
}