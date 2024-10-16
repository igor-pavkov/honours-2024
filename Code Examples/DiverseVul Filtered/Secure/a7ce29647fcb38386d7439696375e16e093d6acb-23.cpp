static void var_field_free(RAnalVarField *field) {
	if (!field) {
		return;
	}
	free (field->name);
	free (field);
}