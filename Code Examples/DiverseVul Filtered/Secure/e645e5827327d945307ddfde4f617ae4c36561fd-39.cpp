static pyc_object *get_string_object(RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	ut32 n = 0;

	n = get_ut32(buffer, &error);
	if (n > ST32_MAX) {
		RZ_LOG_ERROR("bad marshal data (string size out of range)");
		return NULL;
	}
	if (error) {
		return NULL;
	}
	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_STRING;
	ret->data = get_bytes(buffer, n);
	if (!ret->data) {
		RZ_FREE(ret);
		return NULL;
	}
	return ret;
}