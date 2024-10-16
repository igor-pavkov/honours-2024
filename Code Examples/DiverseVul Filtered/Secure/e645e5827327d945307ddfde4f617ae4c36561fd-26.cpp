static pyc_object *get_binary_complex_object(RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	double a, b;

	// a + bj
	a = get_float64(buffer, &error);
	b = get_float64(buffer, &error);
	if (error) {
		return NULL;
	}
	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_BINARY_COMPLEX;
	ret->data = rz_str_newf("%.15g+%.15gj", a, b);
	if (!ret->data) {
		RZ_FREE(ret);
		return NULL;
	}
	return ret;
}