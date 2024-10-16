static pyc_object *get_small_tuple_object(RzBuffer *buffer) {
	pyc_object *ret = NULL;
	bool error = false;
	ut8 n = 0;

	n = get_ut8(buffer, &error);
	if (error) {
		return NULL;
	}
	ret = get_array_object_generic(buffer, n);
	if (ret) {
		ret->type = TYPE_SMALL_TUPLE;
		return ret;
	}
	return NULL;
}