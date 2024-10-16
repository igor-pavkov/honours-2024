static int inode_doinit_with_dentry(struct inode *inode, struct dentry *opt_dentry)
{
	struct superblock_security_struct *sbsec = NULL;
	struct inode_security_struct *isec = inode->i_security;
	u32 task_sid, sid = 0;
	u16 sclass;
	struct dentry *dentry;
#define INITCONTEXTLEN 255
	char *context = NULL;
	unsigned len = 0;
	int rc = 0;

	if (isec->initialized == LABEL_INITIALIZED)
		return 0;

	spin_lock(&isec->lock);
	if (isec->initialized == LABEL_INITIALIZED)
		goto out_unlock;

	if (isec->sclass == SECCLASS_FILE)
		isec->sclass = inode_mode_to_security_class(inode->i_mode);

	sbsec = inode->i_sb->s_security;
	if (!(sbsec->flags & SE_SBINITIALIZED)) {
		/* Defer initialization until selinux_complete_init,
		   after the initial policy is loaded and the security
		   server is ready to handle calls. */
		spin_lock(&sbsec->isec_lock);
		if (list_empty(&isec->list))
			list_add(&isec->list, &sbsec->isec_head);
		spin_unlock(&sbsec->isec_lock);
		goto out_unlock;
	}

	sclass = isec->sclass;
	task_sid = isec->task_sid;
	sid = isec->sid;
	isec->initialized = LABEL_PENDING;
	spin_unlock(&isec->lock);

	switch (sbsec->behavior) {
	case SECURITY_FS_USE_NATIVE:
		break;
	case SECURITY_FS_USE_XATTR:
		if (!(inode->i_opflags & IOP_XATTR)) {
			sid = sbsec->def_sid;
			break;
		}
		/* Need a dentry, since the xattr API requires one.
		   Life would be simpler if we could just pass the inode. */
		if (opt_dentry) {
			/* Called from d_instantiate or d_splice_alias. */
			dentry = dget(opt_dentry);
		} else {
			/* Called from selinux_complete_init, try to find a dentry. */
			dentry = d_find_alias(inode);
		}
		if (!dentry) {
			/*
			 * this is can be hit on boot when a file is accessed
			 * before the policy is loaded.  When we load policy we
			 * may find inodes that have no dentry on the
			 * sbsec->isec_head list.  No reason to complain as these
			 * will get fixed up the next time we go through
			 * inode_doinit with a dentry, before these inodes could
			 * be used again by userspace.
			 */
			goto out;
		}

		len = INITCONTEXTLEN;
		context = kmalloc(len+1, GFP_NOFS);
		if (!context) {
			rc = -ENOMEM;
			dput(dentry);
			goto out;
		}
		context[len] = '\0';
		rc = __vfs_getxattr(dentry, inode, XATTR_NAME_SELINUX, context, len);
		if (rc == -ERANGE) {
			kfree(context);

			/* Need a larger buffer.  Query for the right size. */
			rc = __vfs_getxattr(dentry, inode, XATTR_NAME_SELINUX, NULL, 0);
			if (rc < 0) {
				dput(dentry);
				goto out;
			}
			len = rc;
			context = kmalloc(len+1, GFP_NOFS);
			if (!context) {
				rc = -ENOMEM;
				dput(dentry);
				goto out;
			}
			context[len] = '\0';
			rc = __vfs_getxattr(dentry, inode, XATTR_NAME_SELINUX, context, len);
		}
		dput(dentry);
		if (rc < 0) {
			if (rc != -ENODATA) {
				printk(KERN_WARNING "SELinux: %s:  getxattr returned "
				       "%d for dev=%s ino=%ld\n", __func__,
				       -rc, inode->i_sb->s_id, inode->i_ino);
				kfree(context);
				goto out;
			}
			/* Map ENODATA to the default file SID */
			sid = sbsec->def_sid;
			rc = 0;
		} else {
			rc = security_context_to_sid_default(context, rc, &sid,
							     sbsec->def_sid,
							     GFP_NOFS);
			if (rc) {
				char *dev = inode->i_sb->s_id;
				unsigned long ino = inode->i_ino;

				if (rc == -EINVAL) {
					if (printk_ratelimit())
						printk(KERN_NOTICE "SELinux: inode=%lu on dev=%s was found to have an invalid "
							"context=%s.  This indicates you may need to relabel the inode or the "
							"filesystem in question.\n", ino, dev, context);
				} else {
					printk(KERN_WARNING "SELinux: %s:  context_to_sid(%s) "
					       "returned %d for dev=%s ino=%ld\n",
					       __func__, context, -rc, dev, ino);
				}
				kfree(context);
				/* Leave with the unlabeled SID */
				rc = 0;
				break;
			}
		}
		kfree(context);
		break;
	case SECURITY_FS_USE_TASK:
		sid = task_sid;
		break;
	case SECURITY_FS_USE_TRANS:
		/* Default to the fs SID. */
		sid = sbsec->sid;

		/* Try to obtain a transition SID. */
		rc = security_transition_sid(task_sid, sid, sclass, NULL, &sid);
		if (rc)
			goto out;
		break;
	case SECURITY_FS_USE_MNTPOINT:
		sid = sbsec->mntpoint_sid;
		break;
	default:
		/* Default to the fs superblock SID. */
		sid = sbsec->sid;

		if ((sbsec->flags & SE_SBGENFS) && !S_ISLNK(inode->i_mode)) {
			/* We must have a dentry to determine the label on
			 * procfs inodes */
			if (opt_dentry)
				/* Called from d_instantiate or
				 * d_splice_alias. */
				dentry = dget(opt_dentry);
			else
				/* Called from selinux_complete_init, try to
				 * find a dentry. */
				dentry = d_find_alias(inode);
			/*
			 * This can be hit on boot when a file is accessed
			 * before the policy is loaded.  When we load policy we
			 * may find inodes that have no dentry on the
			 * sbsec->isec_head list.  No reason to complain as
			 * these will get fixed up the next time we go through
			 * inode_doinit() with a dentry, before these inodes
			 * could be used again by userspace.
			 */
			if (!dentry)
				goto out;
			rc = selinux_genfs_get_sid(dentry, sclass,
						   sbsec->flags, &sid);
			dput(dentry);
			if (rc)
				goto out;
		}
		break;
	}

out:
	spin_lock(&isec->lock);
	if (isec->initialized == LABEL_PENDING) {
		if (!sid || rc) {
			isec->initialized = LABEL_INVALID;
			goto out_unlock;
		}

		isec->initialized = LABEL_INITIALIZED;
		isec->sid = sid;
	}

out_unlock:
	spin_unlock(&isec->lock);
	return rc;
}