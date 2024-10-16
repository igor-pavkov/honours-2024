static pyc_object *get_short_ascii_interned_object(RBuffer *buffer) {
	bool error = false;
	ut8 n = get_ut8 (buffer, &error);
	return error? NULL: get_ascii_object_generic (buffer, n, true);
}