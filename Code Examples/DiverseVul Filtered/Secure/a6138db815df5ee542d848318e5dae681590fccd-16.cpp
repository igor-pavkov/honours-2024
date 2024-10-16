static void *mntns_get(struct task_struct *task)
{
	struct mnt_namespace *ns = NULL;
	struct nsproxy *nsproxy;

	task_lock(task);
	nsproxy = task->nsproxy;
	if (nsproxy) {
		ns = nsproxy->mnt_ns;
		get_mnt_ns(ns);
	}
	task_unlock(task);

	return ns;
}