int ecryptfs_add_keysig(struct ecryptfs_crypt_stat *crypt_stat, char *sig)
{
	struct ecryptfs_key_sig *new_key_sig;
	int rc = 0;

	new_key_sig = kmem_cache_alloc(ecryptfs_key_sig_cache, GFP_KERNEL);
	if (!new_key_sig) {
		rc = -ENOMEM;
		printk(KERN_ERR
		       "Error allocating from ecryptfs_key_sig_cache\n");
		goto out;
	}
	memcpy(new_key_sig->keysig, sig, ECRYPTFS_SIG_SIZE_HEX);
	mutex_lock(&crypt_stat->keysig_list_mutex);
	list_add(&new_key_sig->crypt_stat_list, &crypt_stat->keysig_list);
	mutex_unlock(&crypt_stat->keysig_list_mutex);
out:
	return rc;
}