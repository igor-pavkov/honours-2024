static bool parse_domain_user(struct torture_context *torture,
			      const char *domuser, fstring domain,
			      fstring user)
{
	char *p = strchr(domuser, winbind_separator(torture));
	char *dom = NULL;

	if (!p) {
		/* Maybe it was a UPN? */
		if ((p = strchr(domuser, '@')) != NULL) {
			fstrcpy(domain, "");
			fstrcpy(user, domuser);
			return true;
		}

		fstrcpy(user, domuser);
		get_winbind_domain(torture, &dom);
		fstrcpy(domain, dom);
		return true;
	}

	fstrcpy(user, p+1);
	fstrcpy(domain, domuser);
	domain[PTR_DIFF(p, domuser)] = 0;

	return true;
}