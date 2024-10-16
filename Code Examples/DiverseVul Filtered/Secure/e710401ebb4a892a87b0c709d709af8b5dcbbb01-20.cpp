R_API void r_egg_option_set(REgg *egg, const char *key, const char *val) {
	sdb_set (egg->db, key, val, 0);
}