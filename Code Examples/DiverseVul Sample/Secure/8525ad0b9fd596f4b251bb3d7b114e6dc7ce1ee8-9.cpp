static pyc_object *get_ascii_object_generic(RBuffer *buffer, ut32 size, bool interned) {
	pyc_object *ret = R_NEW0 (pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_ASCII;
	ret->data = get_bytes (buffer, size);
	if (!ret->data) {
		R_FREE (ret);
	}
	return ret;
}