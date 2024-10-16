int cgroup1_reconfigure(struct fs_context *fc)
{
	struct cgroup_fs_context *ctx = cgroup_fc2context(fc);
	struct kernfs_root *kf_root = kernfs_root_from_sb(fc->root->d_sb);
	struct cgroup_root *root = cgroup_root_from_kf(kf_root);
	int ret = 0;
	u16 added_mask, removed_mask;

	cgroup_lock_and_drain_offline(&cgrp_dfl_root.cgrp);

	/* See what subsystems are wanted */
	ret = check_cgroupfs_options(fc);
	if (ret)
		goto out_unlock;

	if (ctx->subsys_mask != root->subsys_mask || ctx->release_agent)
		pr_warn("option changes via remount are deprecated (pid=%d comm=%s)\n",
			task_tgid_nr(current), current->comm);

	added_mask = ctx->subsys_mask & ~root->subsys_mask;
	removed_mask = root->subsys_mask & ~ctx->subsys_mask;

	/* Don't allow flags or name to change at remount */
	if ((ctx->flags ^ root->flags) ||
	    (ctx->name && strcmp(ctx->name, root->name))) {
		errorfc(fc, "option or name mismatch, new: 0x%x \"%s\", old: 0x%x \"%s\"",
		       ctx->flags, ctx->name ?: "", root->flags, root->name);
		ret = -EINVAL;
		goto out_unlock;
	}

	/* remounting is not allowed for populated hierarchies */
	if (!list_empty(&root->cgrp.self.children)) {
		ret = -EBUSY;
		goto out_unlock;
	}

	ret = rebind_subsystems(root, added_mask);
	if (ret)
		goto out_unlock;

	WARN_ON(rebind_subsystems(&cgrp_dfl_root, removed_mask));

	if (ctx->release_agent) {
		spin_lock(&release_agent_path_lock);
		strcpy(root->release_agent_path, ctx->release_agent);
		spin_unlock(&release_agent_path_lock);
	}

	trace_cgroup_remount(root);

 out_unlock:
	mutex_unlock(&cgroup_mutex);
	return ret;
}