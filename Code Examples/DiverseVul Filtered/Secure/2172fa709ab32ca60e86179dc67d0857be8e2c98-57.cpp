int security_get_bools(int *len, char ***names, int **values)
{
	int i, rc;

	read_lock(&policy_rwlock);
	*names = NULL;
	*values = NULL;

	rc = 0;
	*len = policydb.p_bools.nprim;
	if (!*len)
		goto out;

	rc = -ENOMEM;
	*names = kcalloc(*len, sizeof(char *), GFP_ATOMIC);
	if (!*names)
		goto err;

	rc = -ENOMEM;
	*values = kcalloc(*len, sizeof(int), GFP_ATOMIC);
	if (!*values)
		goto err;

	for (i = 0; i < *len; i++) {
		size_t name_len;

		(*values)[i] = policydb.bool_val_to_struct[i]->state;
		name_len = strlen(sym_name(&policydb, SYM_BOOLS, i)) + 1;

		rc = -ENOMEM;
		(*names)[i] = kmalloc(sizeof(char) * name_len, GFP_ATOMIC);
		if (!(*names)[i])
			goto err;

		strncpy((*names)[i], sym_name(&policydb, SYM_BOOLS, i), name_len);
		(*names)[i][name_len - 1] = 0;
	}
	rc = 0;
out:
	read_unlock(&policy_rwlock);
	return rc;
err:
	if (*names) {
		for (i = 0; i < *len; i++)
			kfree((*names)[i]);
	}
	kfree(*values);
	goto out;
}