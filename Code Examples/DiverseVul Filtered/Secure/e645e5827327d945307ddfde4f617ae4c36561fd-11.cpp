bool get_sections_symbols_from_code_objects(RzBuffer *buffer, RzList *sections, RzList *symbols, RzList *cobjs, ut32 magic) {
	bool ret;
	magic_int = magic;
	refs = rz_list_newf((RzListFree)free_object);
	if (!refs) {
		return false;
	}
	ret = extract_sections_symbols(get_object(buffer), sections, symbols, cobjs, NULL);
	rz_list_free(refs);
	return ret;
}