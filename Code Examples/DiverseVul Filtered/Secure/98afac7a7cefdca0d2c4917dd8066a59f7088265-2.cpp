static char *get_default_remote(void)
{
	char *dest = NULL, *ret;
	unsigned char sha1[20];
	struct strbuf sb = STRBUF_INIT;
	const char *refname = resolve_ref_unsafe("HEAD", 0, sha1, NULL);

	if (!refname)
		die(_("No such ref: %s"), "HEAD");

	/* detached HEAD */
	if (!strcmp(refname, "HEAD"))
		return xstrdup("origin");

	if (!skip_prefix(refname, "refs/heads/", &refname))
		die(_("Expecting a full ref name, got %s"), refname);

	strbuf_addf(&sb, "branch.%s.remote", refname);
	if (git_config_get_string(sb.buf, &dest))
		ret = xstrdup("origin");
	else
		ret = dest;

	strbuf_release(&sb);
	return ret;
}