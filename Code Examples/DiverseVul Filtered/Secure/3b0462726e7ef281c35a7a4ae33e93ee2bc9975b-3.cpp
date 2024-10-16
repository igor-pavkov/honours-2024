static int cgroup1_root_to_use(struct fs_context *fc)
{
	struct cgroup_fs_context *ctx = cgroup_fc2context(fc);
	struct cgroup_root *root;
	struct cgroup_subsys *ss;
	int i, ret;

	/* First find the desired set of subsystems */
	ret = check_cgroupfs_options(fc);
	if (ret)
		return ret;

	/*
	 * Destruction of cgroup root is asynchronous, so subsystems may
	 * still be dying after the previous unmount.  Let's drain the
	 * dying subsystems.  We just need to ensure that the ones
	 * unmounted previously finish dying and don't care about new ones
	 * starting.  Testing ref liveliness is good enough.
	 */
	for_each_subsys(ss, i) {
		if (!(ctx->subsys_mask & (1 << i)) ||
		    ss->root == &cgrp_dfl_root)
			continue;

		if (!percpu_ref_tryget_live(&ss->root->cgrp.self.refcnt))
			return 1;	/* restart */
		cgroup_put(&ss->root->cgrp);
	}

	for_each_root(root) {
		bool name_match = false;

		if (root == &cgrp_dfl_root)
			continue;

		/*
		 * If we asked for a name then it must match.  Also, if
		 * name matches but sybsys_mask doesn't, we should fail.
		 * Remember whether name matched.
		 */
		if (ctx->name) {
			if (strcmp(ctx->name, root->name))
				continue;
			name_match = true;
		}

		/*
		 * If we asked for subsystems (or explicitly for no
		 * subsystems) then they must match.
		 */
		if ((ctx->subsys_mask || ctx->none) &&
		    (ctx->subsys_mask != root->subsys_mask)) {
			if (!name_match)
				continue;
			return -EBUSY;
		}

		if (root->flags ^ ctx->flags)
			pr_warn("new mount options do not match the existing superblock, will be ignored\n");

		ctx->root = root;
		return 0;
	}

	/*
	 * No such thing, create a new one.  name= matching without subsys
	 * specification is allowed for already existing hierarchies but we
	 * can't create new one without subsys specification.
	 */
	if (!ctx->subsys_mask && !ctx->none)
		return invalfc(fc, "No subsys list or none specified");

	/* Hierarchies may only be created in the initial cgroup namespace. */
	if (ctx->ns != &init_cgroup_ns)
		return -EPERM;

	root = kzalloc(sizeof(*root), GFP_KERNEL);
	if (!root)
		return -ENOMEM;

	ctx->root = root;
	init_cgroup_root(ctx);

	ret = cgroup_setup_root(root, ctx->subsys_mask);
	if (ret)
		cgroup_free_root(root);
	return ret;
}