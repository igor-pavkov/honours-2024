static pyc_object *get_stringref_object(RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	ut32 n = 0;

	n = get_st32(buffer, &error);
	if (n >= rz_list_length(interned_table)) {
		RZ_LOG_ERROR("bad marshal data (string ref out of range)");
		return NULL;
	}
	if (error) {
		return NULL;
	}
	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_STRINGREF;
	ret->data = rz_list_get_n(interned_table, n);
	if (!ret->data) {
		RZ_FREE(ret);
	}
	return ret;
}