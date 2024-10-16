static pyc_object *get_ascii_interned_object(RBuffer *buffer) {
	bool error = false;
	ut32 n = get_ut32 (buffer, &error);
	if (error) {
		return NULL;
	}
	return get_ascii_object_generic (buffer, n, true);
}