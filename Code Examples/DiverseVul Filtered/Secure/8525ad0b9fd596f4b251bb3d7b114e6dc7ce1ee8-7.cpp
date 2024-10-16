static pyc_object *get_float_object(RBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	ut32 size = 0;

	ut8 n = get_ut8 (buffer, &error);
	if (error) {
		return NULL;
	}
	ret = R_NEW0 (pyc_object);
	if (!ret) {
		return NULL;
	}
	ut8 *s = malloc (n + 1);
	if (!s) {
		free (ret);
		return NULL;
	}
	/* object contain string representation of the number */
	size = r_buf_read (buffer, s, n);
	if (size != n) {
		R_FREE (s);
		R_FREE (ret);
		return NULL;
	}
	s[n] = '\0';
	ret->type = TYPE_FLOAT;
	ret->data = s;
	return ret;
}