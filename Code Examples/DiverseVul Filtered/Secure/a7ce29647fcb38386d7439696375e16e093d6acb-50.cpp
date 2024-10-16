R_API RList *r_anal_var_deserialize(const char *ser) {
	r_return_val_if_fail (ser, NULL);
	RList *ret = r_list_newf ((RListFree)r_anal_var_proto_free);
	while (*ser) {
		RAnalVarProt *v = R_NEW0 (RAnalVarProt);
		r_list_append (ret, v);
		if (!v) {
			goto bad_serial;
		}

		// isarg
		switch (*ser) {
		case 't':
			v->isarg = true;
			break;
		case 'f':
			v->isarg = false;
			break;
		default:
			goto bad_serial;
		}
		ser++;

		// kind
		if (!valid_var_kind (*ser)) {
			goto bad_serial;
		}
		v->kind = *ser++;

		// delta
		char *nxt;
		v->delta = strtol (ser, &nxt, 10);
		if ((!v->delta && nxt == ser) || *nxt != ':') {
			goto bad_serial;
		}
		ser = ++nxt;

		// name
		int i;
		for (i = 0; *nxt != ':'; i++) {
			if (*nxt == ',' || !*nxt) {
				goto bad_serial;
			}
			nxt++;
		}
		v->name = r_str_newlen (ser, i);
		if (!v->name) {
			goto bad_serial;
		}
		ser = ++nxt;

		// type
		for (i = 0; *nxt && *nxt != ','; i++) {
			nxt++;
		}
		v->type = r_str_newlen (ser, i);
		if (!v->type) {
			goto bad_serial;
		}
		ser = nxt;
		if (*ser == ',') {
			ser++;
		}
		while (*ser == ' ') {
			ser++;
		}
	}
	return ret;
bad_serial:
	r_list_free (ret);
	return NULL;
}