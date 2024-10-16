R_API void r_bin_field_free(void *_field) {
	RBinField *field = (RBinField*) _field;
	free (field->name);
	free (field->comment);
	free (field->format);
	free (field);
}