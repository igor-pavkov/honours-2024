static int inode_doinit_use_xattr(struct inode *inode, struct dentry *dentry,
				  u32 def_sid, u32 *sid)
{
#define INITCONTEXTLEN 255
	char *context;
	unsigned int len;
	int rc;

	len = INITCONTEXTLEN;
	context = kmalloc(len + 1, GFP_NOFS);
	if (!context)
		return -ENOMEM;

	context[len] = '\0';
	rc = __vfs_getxattr(dentry, inode, XATTR_NAME_SELINUX, context, len);
	if (rc == -ERANGE) {
		kfree(context);

		/* Need a larger buffer.  Query for the right size. */
		rc = __vfs_getxattr(dentry, inode, XATTR_NAME_SELINUX, NULL, 0);
		if (rc < 0)
			return rc;

		len = rc;
		context = kmalloc(len + 1, GFP_NOFS);
		if (!context)
			return -ENOMEM;

		context[len] = '\0';
		rc = __vfs_getxattr(dentry, inode, XATTR_NAME_SELINUX,
				    context, len);
	}
	if (rc < 0) {
		kfree(context);
		if (rc != -ENODATA) {
			pr_warn("SELinux: %s:  getxattr returned %d for dev=%s ino=%ld\n",
				__func__, -rc, inode->i_sb->s_id, inode->i_ino);
			return rc;
		}
		*sid = def_sid;
		return 0;
	}

	rc = security_context_to_sid_default(&selinux_state, context, rc, sid,
					     def_sid, GFP_NOFS);
	if (rc) {
		char *dev = inode->i_sb->s_id;
		unsigned long ino = inode->i_ino;

		if (rc == -EINVAL) {
			pr_notice_ratelimited("SELinux: inode=%lu on dev=%s was found to have an invalid context=%s.  This indicates you may need to relabel the inode or the filesystem in question.\n",
					      ino, dev, context);
		} else {
			pr_warn("SELinux: %s:  context_to_sid(%s) returned %d for dev=%s ino=%ld\n",
				__func__, context, -rc, dev, ino);
		}
	}
	kfree(context);
	return 0;
}