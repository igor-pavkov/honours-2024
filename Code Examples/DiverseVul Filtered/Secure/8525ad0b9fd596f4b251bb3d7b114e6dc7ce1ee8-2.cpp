static pyc_object *get_none_object(void) {
	pyc_object *ret = R_NEW0 (pyc_object);
	if (ret) {
		ret->type = TYPE_NONE;
		ret->data = strdup ("None");
		if (!ret->data) {
			R_FREE (ret);
		}
	}
	return ret;
}