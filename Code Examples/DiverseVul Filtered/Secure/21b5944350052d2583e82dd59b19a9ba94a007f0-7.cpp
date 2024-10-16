static struct ns_common *netns_get(struct task_struct *task)
{
	struct net *net = NULL;
	struct nsproxy *nsproxy;

	task_lock(task);
	nsproxy = task->nsproxy;
	if (nsproxy)
		net = get_net(nsproxy->net_ns);
	task_unlock(task);

	return net ? &net->ns : NULL;
}