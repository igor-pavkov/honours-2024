static pyc_object *get_array_object_generic(RzBuffer *buffer, ut32 size) {
	pyc_object *tmp = NULL;
	pyc_object *ret = NULL;
	ut32 i = 0;

	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->data = rz_list_newf((RzListFree)free_object);
	if (!ret->data) {
		free(ret);
		return NULL;
	}
	for (i = 0; i < size; i++) {
		tmp = get_object(buffer);
		if (!tmp) {
			rz_list_free(ret->data);
			RZ_FREE(ret);
			return NULL;
		}
		if (!rz_list_append(ret->data, tmp)) {
			free_object(tmp);
			rz_list_free(ret->data);
			free(ret);
			return NULL;
		}
	}
	return ret;
}