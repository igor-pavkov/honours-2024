static pyc_object *get_short_ascii_interned_object(RzBuffer *buffer) {
	bool error = false;
	ut8 n;

	n = get_ut8(buffer, &error);
	if (error) {
		return NULL;
	}
	return get_ascii_object_generic(buffer, n, true);
}