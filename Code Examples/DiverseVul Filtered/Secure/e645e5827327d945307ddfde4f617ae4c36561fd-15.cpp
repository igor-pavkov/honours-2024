static pyc_object *get_int_object(RzBuffer *buffer) {
	bool error = false;
	pyc_object *ret = NULL;

	st32 i = get_st32(buffer, &error);
	if (error) {
		return NULL;
	}
	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_INT;
	ret->data = rz_str_newf("%d", i);
	if (!ret->data) {
		RZ_FREE(ret);
	}
	return ret;
}