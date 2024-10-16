static bool parse_namemap(RBuffer *b, ut64 bound, RIDStorage *map, ut32 *count) {
	size_t i;
	if (!consume_u32_r (b, bound, count)) {
		return false;
	}

	for (i = 0; i < *count; i++) {
		ut32 idx;
		if (!consume_u32_r (b, bound, &idx)) {
			return false;
		}

		char *name = NULL;
		if (!consume_encoded_name_new (b, bound, NULL, &name)) {
			R_FREE (name);
			return false;
		}

		if (!r_id_storage_add (map, name, &idx)) {
			R_FREE (name);
			return false;
		};
	}

	return true;
}