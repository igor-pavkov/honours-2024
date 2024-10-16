struct cred *prepare_kernel_cred(struct task_struct *daemon)
{
#ifdef CONFIG_KEYS
	struct thread_group_cred *tgcred;
#endif
	const struct cred *old;
	struct cred *new;

	new = kmem_cache_alloc(cred_jar, GFP_KERNEL);
	if (!new)
		return NULL;

#ifdef CONFIG_KEYS
	tgcred = kmalloc(sizeof(*tgcred), GFP_KERNEL);
	if (!tgcred) {
		kmem_cache_free(cred_jar, new);
		return NULL;
	}
#endif

	kdebug("prepare_kernel_cred() alloc %p", new);

	if (daemon)
		old = get_task_cred(daemon);
	else
		old = get_cred(&init_cred);

	validate_creds(old);

	*new = *old;
	atomic_set(&new->usage, 1);
	set_cred_subscribers(new, 0);
	get_uid(new->user);
	get_group_info(new->group_info);

#ifdef CONFIG_KEYS
	atomic_set(&tgcred->usage, 1);
	spin_lock_init(&tgcred->lock);
	tgcred->process_keyring = NULL;
	tgcred->session_keyring = NULL;
	new->tgcred = tgcred;
	new->request_key_auth = NULL;
	new->thread_keyring = NULL;
	new->jit_keyring = KEY_REQKEY_DEFL_THREAD_KEYRING;
#endif

#ifdef CONFIG_SECURITY
	new->security = NULL;
#endif
	if (security_prepare_creds(new, old, GFP_KERNEL) < 0)
		goto error;

	put_cred(old);
	validate_creds(new);
	return new;

error:
	put_cred(new);
	put_cred(old);
	return NULL;
}