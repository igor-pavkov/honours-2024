int ecryptfs_parse_packet_set(struct ecryptfs_crypt_stat *crypt_stat,
			      unsigned char *src,
			      struct dentry *ecryptfs_dentry)
{
	size_t i = 0;
	size_t found_auth_tok;
	size_t next_packet_is_auth_tok_packet;
	struct list_head auth_tok_list;
	struct ecryptfs_auth_tok *matching_auth_tok;
	struct ecryptfs_auth_tok *candidate_auth_tok;
	char *candidate_auth_tok_sig;
	size_t packet_size;
	struct ecryptfs_auth_tok *new_auth_tok;
	unsigned char sig_tmp_space[ECRYPTFS_SIG_SIZE];
	struct ecryptfs_auth_tok_list_item *auth_tok_list_item;
	size_t tag_11_contents_size;
	size_t tag_11_packet_size;
	int rc = 0;

	INIT_LIST_HEAD(&auth_tok_list);
	/* Parse the header to find as many packets as we can; these will be
	 * added the our &auth_tok_list */
	next_packet_is_auth_tok_packet = 1;
	while (next_packet_is_auth_tok_packet) {
		size_t max_packet_size = ((PAGE_CACHE_SIZE - 8) - i);

		switch (src[i]) {
		case ECRYPTFS_TAG_3_PACKET_TYPE:
			rc = parse_tag_3_packet(crypt_stat,
						(unsigned char *)&src[i],
						&auth_tok_list, &new_auth_tok,
						&packet_size, max_packet_size);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "Error parsing "
						"tag 3 packet\n");
				rc = -EIO;
				goto out_wipe_list;
			}
			i += packet_size;
			rc = parse_tag_11_packet((unsigned char *)&src[i],
						 sig_tmp_space,
						 ECRYPTFS_SIG_SIZE,
						 &tag_11_contents_size,
						 &tag_11_packet_size,
						 max_packet_size);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "No valid "
						"(ecryptfs-specific) literal "
						"packet containing "
						"authentication token "
						"signature found after "
						"tag 3 packet\n");
				rc = -EIO;
				goto out_wipe_list;
			}
			i += tag_11_packet_size;
			if (ECRYPTFS_SIG_SIZE != tag_11_contents_size) {
				ecryptfs_printk(KERN_ERR, "Expected "
						"signature of size [%d]; "
						"read size [%d]\n",
						ECRYPTFS_SIG_SIZE,
						tag_11_contents_size);
				rc = -EIO;
				goto out_wipe_list;
			}
			ecryptfs_to_hex(new_auth_tok->token.password.signature,
					sig_tmp_space, tag_11_contents_size);
			new_auth_tok->token.password.signature[
				ECRYPTFS_PASSWORD_SIG_SIZE] = '\0';
			crypt_stat->flags |= ECRYPTFS_ENCRYPTED;
			break;
		case ECRYPTFS_TAG_1_PACKET_TYPE:
			rc = parse_tag_1_packet(crypt_stat,
						(unsigned char *)&src[i],
						&auth_tok_list, &new_auth_tok,
						&packet_size, max_packet_size);
			if (rc) {
				ecryptfs_printk(KERN_ERR, "Error parsing "
						"tag 1 packet\n");
				rc = -EIO;
				goto out_wipe_list;
			}
			i += packet_size;
			crypt_stat->flags |= ECRYPTFS_ENCRYPTED;
			break;
		case ECRYPTFS_TAG_11_PACKET_TYPE:
			ecryptfs_printk(KERN_WARNING, "Invalid packet set "
					"(Tag 11 not allowed by itself)\n");
			rc = -EIO;
			goto out_wipe_list;
			break;
		default:
			ecryptfs_printk(KERN_DEBUG, "No packet at offset "
					"[%d] of the file header; hex value of "
					"character is [0x%.2x]\n", i, src[i]);
			next_packet_is_auth_tok_packet = 0;
		}
	}
	if (list_empty(&auth_tok_list)) {
		printk(KERN_ERR "The lower file appears to be a non-encrypted "
		       "eCryptfs file; this is not supported in this version "
		       "of the eCryptfs kernel module\n");
		rc = -EINVAL;
		goto out;
	}
	/* auth_tok_list contains the set of authentication tokens
	 * parsed from the metadata. We need to find a matching
	 * authentication token that has the secret component(s)
	 * necessary to decrypt the EFEK in the auth_tok parsed from
	 * the metadata. There may be several potential matches, but
	 * just one will be sufficient to decrypt to get the FEK. */
find_next_matching_auth_tok:
	found_auth_tok = 0;
	list_for_each_entry(auth_tok_list_item, &auth_tok_list, list) {
		candidate_auth_tok = &auth_tok_list_item->auth_tok;
		if (unlikely(ecryptfs_verbosity > 0)) {
			ecryptfs_printk(KERN_DEBUG,
					"Considering cadidate auth tok:\n");
			ecryptfs_dump_auth_tok(candidate_auth_tok);
		}
		rc = ecryptfs_get_auth_tok_sig(&candidate_auth_tok_sig,
					       candidate_auth_tok);
		if (rc) {
			printk(KERN_ERR
			       "Unrecognized candidate auth tok type: [%d]\n",
			       candidate_auth_tok->token_type);
			rc = -EINVAL;
			goto out_wipe_list;
		}
		ecryptfs_find_auth_tok_for_sig(&matching_auth_tok,
					       crypt_stat->mount_crypt_stat,
					       candidate_auth_tok_sig);
		if (matching_auth_tok) {
			found_auth_tok = 1;
			goto found_matching_auth_tok;
		}
	}
	if (!found_auth_tok) {
		ecryptfs_printk(KERN_ERR, "Could not find a usable "
				"authentication token\n");
		rc = -EIO;
		goto out_wipe_list;
	}
found_matching_auth_tok:
	if (candidate_auth_tok->token_type == ECRYPTFS_PRIVATE_KEY) {
		memcpy(&(candidate_auth_tok->token.private_key),
		       &(matching_auth_tok->token.private_key),
		       sizeof(struct ecryptfs_private_key));
		rc = decrypt_pki_encrypted_session_key(candidate_auth_tok,
						       crypt_stat);
	} else if (candidate_auth_tok->token_type == ECRYPTFS_PASSWORD) {
		memcpy(&(candidate_auth_tok->token.password),
		       &(matching_auth_tok->token.password),
		       sizeof(struct ecryptfs_password));
		rc = decrypt_passphrase_encrypted_session_key(
			candidate_auth_tok, crypt_stat);
	}
	if (rc) {
		struct ecryptfs_auth_tok_list_item *auth_tok_list_item_tmp;

		ecryptfs_printk(KERN_WARNING, "Error decrypting the "
				"session key for authentication token with sig "
				"[%.*s]; rc = [%d]. Removing auth tok "
				"candidate from the list and searching for "
				"the next match.\n", candidate_auth_tok_sig,
				ECRYPTFS_SIG_SIZE_HEX, rc);
		list_for_each_entry_safe(auth_tok_list_item,
					 auth_tok_list_item_tmp,
					 &auth_tok_list, list) {
			if (candidate_auth_tok
			    == &auth_tok_list_item->auth_tok) {
				list_del(&auth_tok_list_item->list);
				kmem_cache_free(
					ecryptfs_auth_tok_list_item_cache,
					auth_tok_list_item);
				goto find_next_matching_auth_tok;
			}
		}
		BUG();
	}
	rc = ecryptfs_compute_root_iv(crypt_stat);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error computing "
				"the root IV\n");
		goto out_wipe_list;
	}
	rc = ecryptfs_init_crypt_ctx(crypt_stat);
	if (rc) {
		ecryptfs_printk(KERN_ERR, "Error initializing crypto "
				"context for cipher [%s]; rc = [%d]\n",
				crypt_stat->cipher, rc);
	}
out_wipe_list:
	wipe_auth_tok_list(&auth_tok_list);
out:
	return rc;
}