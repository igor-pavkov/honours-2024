PHP_FUNCTION(openssl_get_curve_names)
{
	EC_builtin_curve *curves = NULL;
	const char *sname;
	size_t i;
	size_t len = EC_get_builtin_curves(NULL, 0);

	curves = emalloc(sizeof(EC_builtin_curve) * len);
	if (!EC_get_builtin_curves(curves, len)) {
		RETURN_FALSE;
	}

	array_init(return_value);
	for (i = 0; i < len; i++) {
		sname = OBJ_nid2sn(curves[i].nid);
		if (sname != NULL) {
			add_next_index_string(return_value, sname);
		}
	}
	efree(curves);
}