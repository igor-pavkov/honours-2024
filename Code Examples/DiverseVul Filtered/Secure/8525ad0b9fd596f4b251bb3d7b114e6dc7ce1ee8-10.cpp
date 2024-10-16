static pyc_object *get_dict_object(RBuffer *buffer) {
	pyc_object *key = NULL,
		   *val = NULL;

	pyc_object *ret = R_NEW0 (pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->data = r_list_newf ((RListFree)free_object);
	if (!ret->data) {
		R_FREE (ret);
		return NULL;
	}
	for (;;) {
		key = get_object (buffer);
		if (!key) {
			break;
		}
		if (!r_list_append (ret->data, key)) {
			r_list_free (ret->data);
			R_FREE (ret);
			free_object (key);
			return NULL;
		}
		val = get_object (buffer);
		if (!val) {
			break;
		}
		if (!r_list_append (ret->data, val)) {
			free_object (val);
			r_list_free (ret->data);
			R_FREE (ret);
			return NULL;
		}
	}
	ret->type = TYPE_DICT;
	return ret;
}