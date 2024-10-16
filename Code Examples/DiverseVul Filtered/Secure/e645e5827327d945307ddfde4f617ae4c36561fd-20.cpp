static pyc_object *get_dict_object(RzBuffer *buffer) {
	pyc_object *ret = NULL,
		   *key = NULL,
		   *val = NULL;

	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->data = rz_list_newf((RzListFree)free_object);
	if (!ret->data) {
		RZ_FREE(ret);
		return NULL;
	}
	for (;;) {
		key = get_object(buffer);
		if (!key) {
			break;
		}
		if (!rz_list_append(ret->data, key)) {
			rz_list_free(ret->data);
			RZ_FREE(ret);
			free_object(key);
			return NULL;
		}
		val = get_object(buffer);
		if (!val) {
			break;
		}
		if (!rz_list_append(ret->data, val)) {
			rz_list_free(ret->data);
			RZ_FREE(ret);
			free_object(val);
			return NULL;
		}
	}
	ret->type = TYPE_DICT;
	return ret;
}