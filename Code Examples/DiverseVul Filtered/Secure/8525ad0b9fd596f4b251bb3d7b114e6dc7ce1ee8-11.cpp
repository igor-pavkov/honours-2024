static pyc_object *get_short_ascii_object(RBuffer *buffer) {
	bool error = false;
	ut8 n = get_ut8 (buffer, &error);
	if (error) {
		return NULL;
	}
	return get_ascii_object_generic (buffer, n, false);
}