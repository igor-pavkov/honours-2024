static pyc_object *get_int64_object(RBuffer *buffer) {
	bool error = false;
	st64 i = get_st64 (buffer, &error);

	if (error) {
		return NULL;
	}
	pyc_object *ret = R_NEW0 (pyc_object);
	if (ret) {
		ret->type = TYPE_INT64;
		ret->data = r_str_newf ("%"PFMT64d, (st64)i);
		if (!ret->data) {
			R_FREE (ret);
		}
	}
	return ret;
}