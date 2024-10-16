static pyc_object *get_ascii_object_generic(RzBuffer *buffer, ut32 size, bool interned) {
	pyc_object *ret = NULL;

	ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_ASCII;
	ret->data = get_bytes(buffer, size);
	if (!ret->data) {
		RZ_FREE(ret);
	}
	return ret;
}