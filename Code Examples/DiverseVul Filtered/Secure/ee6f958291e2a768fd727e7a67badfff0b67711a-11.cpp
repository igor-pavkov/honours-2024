static void chroot_fs_refs(struct nameidata *old_nd, struct nameidata *new_nd)
{
	struct task_struct *g, *p;
	struct fs_struct *fs;

	read_lock(&tasklist_lock);
	do_each_thread(g, p) {
		task_lock(p);
		fs = p->fs;
		if (fs) {
			atomic_inc(&fs->count);
			task_unlock(p);
			if (fs->root == old_nd->dentry
			    && fs->rootmnt == old_nd->mnt)
				set_fs_root(fs, new_nd->mnt, new_nd->dentry);
			if (fs->pwd == old_nd->dentry
			    && fs->pwdmnt == old_nd->mnt)
				set_fs_pwd(fs, new_nd->mnt, new_nd->dentry);
			put_fs_struct(fs);
		} else
			task_unlock(p);
	} while_each_thread(g, p);
	read_unlock(&tasklist_lock);
}