static pyc_object *get_int_object(RBuffer *buffer) {
	bool error = false;

	st32 i = get_st32 (buffer, &error);
	if (error) {
		return NULL;
	}
	pyc_object *ret = R_NEW0 (pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_INT;
	ret->data = r_str_newf ("%d", i);
	if (!ret->data) {
		R_FREE (ret);
	}
	return ret;
}