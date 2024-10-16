static pyc_object *get_long_object(RBuffer *buffer) {
	bool error = false;
	bool neg = false;
	ut32 tmp = 0;
	size_t size;
	size_t i, j = 0, left = 0;
	ut16 n;
	char *hexstr;
	char digist2hex[] = "0123456789abcdef";

	st32 ndigits = get_st32 (buffer, &error);
	if (error) {
		return NULL;
	}
	pyc_object *ret = R_NEW0 (pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_LONG;
	if (ndigits < 0) {
		ndigits = -ndigits;
		neg = true;
	}
	if (ndigits == 0) {
		ret->data = strdup ("0x0");
	} else {
		if (ndigits > 10) {
			free (ret);
			return NULL;
		}
		size = ndigits * 15;
		if (size < 0) {
			return NULL;
		}
		size = (size - 1) / 4 + 1;
		if (size < 1) {
			free (ret);
			return NULL;
		}
		size += 3 + (neg? 1: 0);
		j = size - 1;
		hexstr = calloc (size, sizeof (char));
		if (!hexstr) {
			free (ret);
			return NULL;
		}

		for (i = 0; i < ndigits; i++) {
			n = get_ut16 (buffer, &error);
			tmp |= n << left;
			left += 15;

			while (left >= 4 && j >= 0) {
				hexstr[--j] = digist2hex[tmp & 0xf];
				tmp >>= 4;
				left -= 4;
			}
		}

		if (tmp) {
			hexstr[--j] = digist2hex[tmp & 0xf];
		}

		if (j > 0) {
			hexstr[--j] = 'x';
		}
		if (j > 0) {
			hexstr[--j] = '0';
		}
		if (neg && j > 0) {
			hexstr[--j] = '-';
		}

		ret->data = &hexstr[j];
	}
	return ret;
}