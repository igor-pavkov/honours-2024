static pyc_object *get_ref_object(RBuffer *buffer) {
	bool error = false;
	ut32 index = get_ut32 (buffer, &error);
	if (error) {
		return NULL;
	}
	if (index >= r_list_length (refs)) {
		return NULL;
	}
	pyc_object *obj = r_list_get_n (refs, index);
	return obj? copy_object (obj): NULL;
}