void cgroup1_release_agent(struct work_struct *work)
{
	struct cgroup *cgrp =
		container_of(work, struct cgroup, release_agent_work);
	char *pathbuf, *agentbuf;
	char *argv[3], *envp[3];
	int ret;

	/* snoop agent path and exit early if empty */
	if (!cgrp->root->release_agent_path[0])
		return;

	/* prepare argument buffers */
	pathbuf = kmalloc(PATH_MAX, GFP_KERNEL);
	agentbuf = kmalloc(PATH_MAX, GFP_KERNEL);
	if (!pathbuf || !agentbuf)
		goto out_free;

	spin_lock(&release_agent_path_lock);
	strlcpy(agentbuf, cgrp->root->release_agent_path, PATH_MAX);
	spin_unlock(&release_agent_path_lock);
	if (!agentbuf[0])
		goto out_free;

	ret = cgroup_path_ns(cgrp, pathbuf, PATH_MAX, &init_cgroup_ns);
	if (ret < 0 || ret >= PATH_MAX)
		goto out_free;

	argv[0] = agentbuf;
	argv[1] = pathbuf;
	argv[2] = NULL;

	/* minimal command environment */
	envp[0] = "HOME=/";
	envp[1] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
	envp[2] = NULL;

	call_usermodehelper(argv[0], argv, envp, UMH_WAIT_EXEC);
out_free:
	kfree(agentbuf);
	kfree(pathbuf);
}