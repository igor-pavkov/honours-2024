static bool var_add_structure_fields_to_list(RAnal *a, RAnalVar *av, RList *list) {
	Sdb *TDB = a->sdb_types;
	const char *type_kind = sdb_const_get (TDB, av->type, 0);
	if (type_kind && !strcmp (type_kind, "struct")) {
		char *field_name, *new_name;
		int field_n;
		char *type_key = r_str_newf ("%s.%s", type_kind, av->type);
		for (field_n = 0; (field_name = sdb_array_get (TDB, type_key, field_n, NULL)); field_n++) {
			char *field_key = r_str_newf ("%s.%s", type_key, field_name);
			char *field_type = sdb_array_get (TDB, field_key, 0, NULL);
			ut64 field_offset = sdb_array_get_num (TDB, field_key, 1, NULL);
			new_name = r_str_newf ("%s.%s", av->name, field_name);
			RAnalVarField *field = R_NEW0 (RAnalVarField);
			field->name = new_name;
			field->delta = av->delta + field_offset;
			field->field = true;
			r_list_append (list, field);
			free (field_type);
			free (field_key);
			free (field_name);
		}
		free (type_key);
		return true;
	}
	return false;
}