static void shadow_var_struct_members(RAnalVar *var) {
	Sdb *TDB = var->fcn->anal->sdb_types;
	const char *type_kind = sdb_const_get (TDB, var->type, 0);
	if (type_kind && r_str_startswith (type_kind, "struct")) {
		char *field;
		int field_n;
		char *type_key = r_str_newf ("%s.%s", type_kind, var->type);
		for (field_n = 0; (field = sdb_array_get (TDB, type_key, field_n, NULL)); field_n++) {
			char field_key[0x300];
			if (snprintf (field_key, sizeof (field_key), "%s.%s", type_key, field) < 0) {
				continue;
			}
			char *field_type = sdb_array_get (TDB, field_key, 0, NULL);
			ut64 field_offset = sdb_array_get_num (TDB, field_key, 1, NULL);
			if (field_offset != 0) { // delete variables which are overlaid by structure
				RAnalVar *other = r_anal_function_get_var (var->fcn, var->kind, var->delta + field_offset);
				if (other && other != var) {
					r_anal_var_delete (other);
				}
			}
			free (field_type);
			free (field);
		}
		free (type_key);
	}
}