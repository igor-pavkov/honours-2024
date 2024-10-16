static pyc_object *get_interned_object(RzBuffer *buffer) {
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
	ret->type = TYPE_INTERNED;
	ret->data = get_bytes(buffer, n);
	/* add data pointer to interned table */
	rz_list_append(interned_table, ret->data);
	if (!ret->data) {
		RZ_FREE(ret);
	}
	return ret;
}