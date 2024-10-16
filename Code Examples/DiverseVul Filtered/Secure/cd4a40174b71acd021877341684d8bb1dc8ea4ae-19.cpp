bool __is_local_mountpoint(struct dentry *dentry)
{
	struct mnt_namespace *ns = current->nsproxy->mnt_ns;
	struct mount *mnt;
	bool is_covered = false;

	if (!d_mountpoint(dentry))
		goto out;

	down_read(&namespace_sem);
	list_for_each_entry(mnt, &ns->list, mnt_list) {
		is_covered = (mnt->mnt_mountpoint == dentry);
		if (is_covered)
			break;
	}
	up_read(&namespace_sem);
out:
	return is_covered;
}