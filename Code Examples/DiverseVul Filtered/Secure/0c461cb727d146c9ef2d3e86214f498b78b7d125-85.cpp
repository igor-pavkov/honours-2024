static int selinux_inode_copy_up(struct dentry *src, struct cred **new)
{
	u32 sid;
	struct task_security_struct *tsec;
	struct cred *new_creds = *new;

	if (new_creds == NULL) {
		new_creds = prepare_creds();
		if (!new_creds)
			return -ENOMEM;
	}

	tsec = new_creds->security;
	/* Get label from overlay inode and set it in create_sid */
	selinux_inode_getsecid(d_inode(src), &sid);
	tsec->create_sid = sid;
	*new = new_creds;
	return 0;
}