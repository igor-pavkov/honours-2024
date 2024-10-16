static pyc_object *get_unicode_object(RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	ut32 n = 0;

	n = get_ut32(buffer, &error);
	if (n > ST32_MAX) {
		RZ_LOG_ERROR("bad marshal data (unicode size out of range)");
		return NULL;
	}
	if (error) {
		return NULL;
	}
	ret = RZ_NEW0(pyc_object);
	ret->type = TYPE_UNICODE;
	ret->data = get_bytes(buffer, n);
	if (!ret->data) {
		RZ_FREE(ret);
		return NULL;
	}
	return ret;
}