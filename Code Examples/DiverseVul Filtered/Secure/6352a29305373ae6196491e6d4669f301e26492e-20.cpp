int ecryptfs_keyring_auth_tok_for_sig(struct key **auth_tok_key,
				      struct ecryptfs_auth_tok **auth_tok,
				      char *sig)
{
	int rc = 0;

	(*auth_tok_key) = request_key(&key_type_user, sig, NULL);
	if (!(*auth_tok_key) || IS_ERR(*auth_tok_key)) {
		printk(KERN_ERR "Could not find key with description: [%s]\n",
		       sig);
		rc = process_request_key_err(PTR_ERR(*auth_tok_key));
		goto out;
	}
	(*auth_tok) = ecryptfs_get_key_payload_data(*auth_tok_key);
	if (ecryptfs_verify_version((*auth_tok)->version)) {
		printk(KERN_ERR
		       "Data structure version mismatch. "
		       "Userspace tools must match eCryptfs "
		       "kernel module with major version [%d] "
		       "and minor version [%d]\n",
		       ECRYPTFS_VERSION_MAJOR,
		       ECRYPTFS_VERSION_MINOR);
		rc = -EINVAL;
		goto out;
	}
	if ((*auth_tok)->token_type != ECRYPTFS_PASSWORD
	    && (*auth_tok)->token_type != ECRYPTFS_PRIVATE_KEY) {
		printk(KERN_ERR "Invalid auth_tok structure "
		       "returned from key query\n");
		rc = -EINVAL;
		goto out;
	}
out:
	return rc;
}