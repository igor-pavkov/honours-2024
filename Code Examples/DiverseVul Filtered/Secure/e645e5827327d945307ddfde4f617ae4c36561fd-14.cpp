static pyc_object *get_ref_object(RzBuffer *buffer) {
	bool error = false;
	pyc_object *ret;
	pyc_object *obj;
	ut32 index;

	index = get_ut32(buffer, &error);
	if (error) {
		return NULL;
	}
	if (index >= rz_list_length(refs)) {
		return NULL;
	}
	obj = rz_list_get_n(refs, index);
	if (!obj) {
		return NULL;
	}
	ret = copy_object(obj);
	return ret;
}