int security_genfs_sid(const char *fstype,
		       char *path,
		       u16 orig_sclass,
		       u32 *sid)
{
	int len;
	u16 sclass;
	struct genfs *genfs;
	struct ocontext *c;
	int rc, cmp = 0;

	while (path[0] == '/' && path[1] == '/')
		path++;

	read_lock(&policy_rwlock);

	sclass = unmap_class(orig_sclass);
	*sid = SECINITSID_UNLABELED;

	for (genfs = policydb.genfs; genfs; genfs = genfs->next) {
		cmp = strcmp(fstype, genfs->fstype);
		if (cmp <= 0)
			break;
	}

	rc = -ENOENT;
	if (!genfs || cmp)
		goto out;

	for (c = genfs->head; c; c = c->next) {
		len = strlen(c->u.name);
		if ((!c->v.sclass || sclass == c->v.sclass) &&
		    (strncmp(c->u.name, path, len) == 0))
			break;
	}

	rc = -ENOENT;
	if (!c)
		goto out;

	if (!c->sid[0]) {
		rc = sidtab_context_to_sid(&sidtab, &c->context[0], &c->sid[0]);
		if (rc)
			goto out;
	}

	*sid = c->sid[0];
	rc = 0;
out:
	read_unlock(&policy_rwlock);
	return rc;
}