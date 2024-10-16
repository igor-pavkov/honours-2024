int security_sid_mls_copy(u32 sid, u32 mls_sid, u32 *new_sid)
{
	struct context *context1;
	struct context *context2;
	struct context newcon;
	char *s;
	u32 len;
	int rc;

	rc = 0;
	if (!ss_initialized || !policydb.mls_enabled) {
		*new_sid = sid;
		goto out;
	}

	context_init(&newcon);

	read_lock(&policy_rwlock);

	rc = -EINVAL;
	context1 = sidtab_search(&sidtab, sid);
	if (!context1) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
			__func__, sid);
		goto out_unlock;
	}

	rc = -EINVAL;
	context2 = sidtab_search(&sidtab, mls_sid);
	if (!context2) {
		printk(KERN_ERR "SELinux: %s:  unrecognized SID %d\n",
			__func__, mls_sid);
		goto out_unlock;
	}

	newcon.user = context1->user;
	newcon.role = context1->role;
	newcon.type = context1->type;
	rc = mls_context_cpy(&newcon, context2);
	if (rc)
		goto out_unlock;

	/* Check the validity of the new context. */
	if (!policydb_context_isvalid(&policydb, &newcon)) {
		rc = convert_context_handle_invalid_context(&newcon);
		if (rc) {
			if (!context_struct_to_string(&newcon, &s, &len)) {
				audit_log(current->audit_context, GFP_ATOMIC, AUDIT_SELINUX_ERR,
					  "security_sid_mls_copy: invalid context %s", s);
				kfree(s);
			}
			goto out_unlock;
		}
	}

	rc = sidtab_context_to_sid(&sidtab, &newcon, new_sid);
out_unlock:
	read_unlock(&policy_rwlock);
	context_destroy(&newcon);
out:
	return rc;
}