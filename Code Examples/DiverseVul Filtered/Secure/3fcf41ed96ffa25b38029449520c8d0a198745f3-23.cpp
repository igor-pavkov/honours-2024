static int is_data_section(RBinFile *a, RBinSection *s) {
	if (s->has_strings || s->is_data) {
		return true;
	}
	if (s->is_data) {
		return true;
	}
 	// Rust
	return (strstr (s->name, "_const") != NULL);
}