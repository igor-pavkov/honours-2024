ut64 get_code_object_addr(RzBinPycObj *pyc, RzBuffer *buffer, ut32 magic) {
	pyc->magic_int = magic;
	pyc_object *co = get_code_object(pyc, buffer);
	ut64 result = 0;
	if (!co) {
		return 0;
	}

	pyc_code_object *cobj = co->data;
	result = cobj->start_offset;
	free_object(co);

	return result;
}