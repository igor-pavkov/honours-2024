static int check_cgroupfs_options(struct fs_context *fc)
{
	struct cgroup_fs_context *ctx = cgroup_fc2context(fc);
	u16 mask = U16_MAX;
	u16 enabled = 0;
	struct cgroup_subsys *ss;
	int i;

#ifdef CONFIG_CPUSETS
	mask = ~((u16)1 << cpuset_cgrp_id);
#endif
	for_each_subsys(ss, i)
		if (cgroup_ssid_enabled(i) && !cgroup1_ssid_disabled(i))
			enabled |= 1 << i;

	ctx->subsys_mask &= enabled;

	/*
	 * In absence of 'none', 'name=' and subsystem name options,
	 * let's default to 'all'.
	 */
	if (!ctx->subsys_mask && !ctx->none && !ctx->name)
		ctx->all_ss = true;

	if (ctx->all_ss) {
		/* Mutually exclusive option 'all' + subsystem name */
		if (ctx->subsys_mask)
			return invalfc(fc, "subsys name conflicts with all");
		/* 'all' => select all the subsystems */
		ctx->subsys_mask = enabled;
	}

	/*
	 * We either have to specify by name or by subsystems. (So all
	 * empty hierarchies must have a name).
	 */
	if (!ctx->subsys_mask && !ctx->name)
		return invalfc(fc, "Need name or subsystem set");

	/*
	 * Option noprefix was introduced just for backward compatibility
	 * with the old cpuset, so we allow noprefix only if mounting just
	 * the cpuset subsystem.
	 */
	if ((ctx->flags & CGRP_ROOT_NOPREFIX) && (ctx->subsys_mask & mask))
		return invalfc(fc, "noprefix used incorrectly");

	/* Can't specify "none" and some subsystems */
	if (ctx->subsys_mask && ctx->none)
		return invalfc(fc, "none used incorrectly");

	return 0;
}