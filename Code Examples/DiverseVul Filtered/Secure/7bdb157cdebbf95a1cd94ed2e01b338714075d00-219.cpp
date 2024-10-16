static int perf_copy_attr(struct perf_event_attr __user *uattr,
			  struct perf_event_attr *attr)
{
	u32 size;
	int ret;

	/* Zero the full structure, so that a short copy will be nice. */
	memset(attr, 0, sizeof(*attr));

	ret = get_user(size, &uattr->size);
	if (ret)
		return ret;

	/* ABI compatibility quirk: */
	if (!size)
		size = PERF_ATTR_SIZE_VER0;
	if (size < PERF_ATTR_SIZE_VER0 || size > PAGE_SIZE)
		goto err_size;

	ret = copy_struct_from_user(attr, sizeof(*attr), uattr, size);
	if (ret) {
		if (ret == -E2BIG)
			goto err_size;
		return ret;
	}

	attr->size = size;

	if (attr->__reserved_1 || attr->__reserved_2 || attr->__reserved_3)
		return -EINVAL;

	if (attr->sample_type & ~(PERF_SAMPLE_MAX-1))
		return -EINVAL;

	if (attr->read_format & ~(PERF_FORMAT_MAX-1))
		return -EINVAL;

	if (attr->sample_type & PERF_SAMPLE_BRANCH_STACK) {
		u64 mask = attr->branch_sample_type;

		/* only using defined bits */
		if (mask & ~(PERF_SAMPLE_BRANCH_MAX-1))
			return -EINVAL;

		/* at least one branch bit must be set */
		if (!(mask & ~PERF_SAMPLE_BRANCH_PLM_ALL))
			return -EINVAL;

		/* propagate priv level, when not set for branch */
		if (!(mask & PERF_SAMPLE_BRANCH_PLM_ALL)) {

			/* exclude_kernel checked on syscall entry */
			if (!attr->exclude_kernel)
				mask |= PERF_SAMPLE_BRANCH_KERNEL;

			if (!attr->exclude_user)
				mask |= PERF_SAMPLE_BRANCH_USER;

			if (!attr->exclude_hv)
				mask |= PERF_SAMPLE_BRANCH_HV;
			/*
			 * adjust user setting (for HW filter setup)
			 */
			attr->branch_sample_type = mask;
		}
		/* privileged levels capture (kernel, hv): check permissions */
		if (mask & PERF_SAMPLE_BRANCH_PERM_PLM) {
			ret = perf_allow_kernel(attr);
			if (ret)
				return ret;
		}
	}

	if (attr->sample_type & PERF_SAMPLE_REGS_USER) {
		ret = perf_reg_validate(attr->sample_regs_user);
		if (ret)
			return ret;
	}

	if (attr->sample_type & PERF_SAMPLE_STACK_USER) {
		if (!arch_perf_have_user_stack_dump())
			return -ENOSYS;

		/*
		 * We have __u32 type for the size, but so far
		 * we can only use __u16 as maximum due to the
		 * __u16 sample size limit.
		 */
		if (attr->sample_stack_user >= USHRT_MAX)
			return -EINVAL;
		else if (!IS_ALIGNED(attr->sample_stack_user, sizeof(u64)))
			return -EINVAL;
	}

	if (!attr->sample_max_stack)
		attr->sample_max_stack = sysctl_perf_event_max_stack;

	if (attr->sample_type & PERF_SAMPLE_REGS_INTR)
		ret = perf_reg_validate(attr->sample_regs_intr);

#ifndef CONFIG_CGROUP_PERF
	if (attr->sample_type & PERF_SAMPLE_CGROUP)
		return -EINVAL;
#endif

out:
	return ret;

err_size:
	put_user(sizeof(*attr), &uattr->size);
	ret = -E2BIG;
	goto out;
}