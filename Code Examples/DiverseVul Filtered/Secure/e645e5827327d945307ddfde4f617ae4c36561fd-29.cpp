static pyc_object *get_false_object(void) {
	pyc_object *ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_FALSE;
	ret->data = strdup("False");
	if (!ret->data) {
		RZ_FREE(ret);
	}
	return ret;
}