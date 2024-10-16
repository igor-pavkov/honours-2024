static int cgroup1_show_options(struct seq_file *seq, struct kernfs_root *kf_root)
{
	struct cgroup_root *root = cgroup_root_from_kf(kf_root);
	struct cgroup_subsys *ss;
	int ssid;

	for_each_subsys(ss, ssid)
		if (root->subsys_mask & (1 << ssid))
			seq_show_option(seq, ss->legacy_name, NULL);
	if (root->flags & CGRP_ROOT_NOPREFIX)
		seq_puts(seq, ",noprefix");
	if (root->flags & CGRP_ROOT_XATTR)
		seq_puts(seq, ",xattr");
	if (root->flags & CGRP_ROOT_CPUSET_V2_MODE)
		seq_puts(seq, ",cpuset_v2_mode");

	spin_lock(&release_agent_path_lock);
	if (strlen(root->release_agent_path))
		seq_show_option(seq, "release_agent",
				root->release_agent_path);
	spin_unlock(&release_agent_path_lock);

	if (test_bit(CGRP_CPUSET_CLONE_CHILDREN, &root->cgrp.flags))
		seq_puts(seq, ",clone_children");
	if (strlen(root->name))
		seq_show_option(seq, "name", root->name);
	return 0;
}