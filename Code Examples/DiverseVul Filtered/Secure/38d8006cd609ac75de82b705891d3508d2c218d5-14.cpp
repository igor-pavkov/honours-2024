bool get_sections_symbols_from_code_objects(RzBinPycObj *pyc, RzBuffer *buffer, RzList *sections, RzList *symbols, RzList *cobjs, ut32 magic) {
	bool ret;
	pyc->magic_int = magic;
	pyc->refs = rz_list_newf((RzListFree)free_object);
	if (!pyc->refs) {
		return false;
	}
	ret = extract_sections_symbols(pyc, get_object(pyc, buffer), sections, symbols, cobjs, NULL);
	rz_list_free(pyc->refs);
	return ret;
}