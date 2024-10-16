R_API char *r_egg_option_get(REgg *egg, const char *key) {
	return sdb_get (egg->db, key, NULL);
}