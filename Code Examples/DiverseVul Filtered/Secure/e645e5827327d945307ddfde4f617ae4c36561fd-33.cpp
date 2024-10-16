static pyc_object *get_binary_float_object(RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	double f;

	f = get_float64(buffer, &error);
	if (error) {
		return NULL;
	}
	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_FLOAT;
	ret->data = rz_str_newf("%.15g", f);
	if (!ret->data) {
		RZ_FREE(ret);
		return NULL;
	}
	return ret;
}