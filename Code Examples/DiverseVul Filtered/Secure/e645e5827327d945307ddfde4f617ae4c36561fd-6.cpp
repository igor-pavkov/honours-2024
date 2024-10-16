static pyc_object *get_ascii_interned_object(RzBuffer *buffer) {
	bool error = false;
	ut32 n;

	n = get_ut32(buffer, &error);
	if (error) {
		return NULL;
	}
	return get_ascii_object_generic(buffer, n, true);
}