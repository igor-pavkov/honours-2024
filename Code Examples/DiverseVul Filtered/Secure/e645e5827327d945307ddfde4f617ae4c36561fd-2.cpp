static pyc_object *get_int64_object(RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	st64 i;

	i = get_st64(buffer, &error);

	if (error) {
		return NULL;
	}
	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_INT64;
	ret->data = rz_str_newf("%lld", i);
	if (!ret->data) {
		RZ_FREE(ret);
	}
	return ret;
}