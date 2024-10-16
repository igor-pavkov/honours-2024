static void proc_flush_task_mnt(struct vfsmount *mnt, pid_t pid, pid_t tgid)
{
	struct dentry *dentry, *leader, *dir;
	char buf[PROC_NUMBUF];
	struct qstr name;

	name.name = buf;
	name.len = snprintf(buf, sizeof(buf), "%d", pid);
	/* no ->d_hash() rejects on procfs */
	dentry = d_hash_and_lookup(mnt->mnt_root, &name);
	if (dentry) {
		d_invalidate(dentry);
		dput(dentry);
	}

	if (pid == tgid)
		return;

	name.name = buf;
	name.len = snprintf(buf, sizeof(buf), "%d", tgid);
	leader = d_hash_and_lookup(mnt->mnt_root, &name);
	if (!leader)
		goto out;

	name.name = "task";
	name.len = strlen(name.name);
	dir = d_hash_and_lookup(leader, &name);
	if (!dir)
		goto out_put_leader;

	name.name = buf;
	name.len = snprintf(buf, sizeof(buf), "%d", pid);
	dentry = d_hash_and_lookup(dir, &name);
	if (dentry) {
		d_invalidate(dentry);
		dput(dentry);
	}

	dput(dir);
out_put_leader:
	dput(leader);
out:
	return;
}