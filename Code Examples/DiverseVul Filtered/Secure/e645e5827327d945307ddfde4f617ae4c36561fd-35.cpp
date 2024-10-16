static pyc_object *get_true_object(void) {
	pyc_object *ret = RZ_NEW0(pyc_object);
	if (!ret) {
		return NULL;
	}
	ret->type = TYPE_TRUE;
	ret->data = strdup("True");
	if (!ret->data) {
		RZ_FREE(ret);
	}
	return ret;
}