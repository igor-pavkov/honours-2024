struct cred *cred_alloc_blank(void)
{
	struct cred *new;

	new = kmem_cache_zalloc(cred_jar, GFP_KERNEL);
	if (!new)
		return NULL;

#ifdef CONFIG_KEYS
	new->tgcred = kzalloc(sizeof(*new->tgcred), GFP_KERNEL);
	if (!new->tgcred) {
		kmem_cache_free(cred_jar, new);
		return NULL;
	}
	atomic_set(&new->tgcred->usage, 1);
#endif

	atomic_set(&new->usage, 1);
#ifdef CONFIG_DEBUG_CREDENTIALS
	new->magic = CRED_MAGIC;
#endif

	if (security_cred_alloc_blank(new, GFP_KERNEL) < 0)
		goto error;

	return new;

error:
	abort_creds(new);
	return NULL;
}