static char *swiftField(const char *dn, const char *cn) {
	char *p = strstr (dn, ".getter_");
	if (!p) {
		p = strstr (dn, ".setter_");
		if (!p) {
			p = strstr (dn, ".method_");
		}
	}
	if (p) {
		char *q = strstr (dn, cn);
		if (q && q[strlen (cn)] == '.') {
			q = strdup (q + strlen (cn) + 1);
			char *r = strchr (q, '.');
			if (r) {
				*r = 0;
			}
			return q;
		}
	}
	return NULL;
}