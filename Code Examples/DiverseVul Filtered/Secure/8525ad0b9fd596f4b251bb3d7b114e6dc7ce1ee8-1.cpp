static pyc_object *get_stringref_object(RBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	ut32 n = get_st32 (buffer, &error);
	if (n >= r_list_length (interned_table)) {
		eprintf ("bad marshal data (string ref out of range)");
		return NULL;
	}
	if (error) {
		return NULL;
	}
	ret = R_NEW0 (pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_STRINGREF;
	ret->data = r_list_get_n (interned_table, n);
	if (!ret->data) {
		R_FREE (ret);
	}
	return ret;
}