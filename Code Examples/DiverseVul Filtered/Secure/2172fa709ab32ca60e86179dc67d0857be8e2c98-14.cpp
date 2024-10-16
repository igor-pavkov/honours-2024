int security_read_policy(void **data, size_t *len)
{
	int rc;
	struct policy_file fp;

	if (!ss_initialized)
		return -EINVAL;

	*len = security_policydb_len();

	*data = vmalloc_user(*len);
	if (!*data)
		return -ENOMEM;

	fp.data = *data;
	fp.len = *len;

	read_lock(&policy_rwlock);
	rc = policydb_write(&policydb, &fp);
	read_unlock(&policy_rwlock);

	if (rc)
		return rc;

	*len = (unsigned long)fp.data - (unsigned long)*data;
	return 0;

}