static bool set_process_capability(enum smbd_capability capability,
				   bool enable)
{
	cap_value_t cap_vals[2] = {0};
	int num_cap_vals = 0;

	cap_t cap;

#if defined(HAVE_PRCTL) && defined(PR_GET_KEEPCAPS) && defined(PR_SET_KEEPCAPS)
	/* On Linux, make sure that any capabilities we grab are sticky
	 * across UID changes. We expect that this would allow us to keep both
	 * the effective and permitted capability sets, but as of circa 2.6.16,
	 * only the permitted set is kept. It is a bug (which we work around)
	 * that the effective set is lost, but we still require the effective
	 * set to be kept.
	 */
	if (!prctl(PR_GET_KEEPCAPS)) {
		prctl(PR_SET_KEEPCAPS, 1);
	}
#endif

	cap = cap_get_proc();
	if (cap == NULL) {
		DEBUG(0,("set_process_capability: cap_get_proc failed: %s\n",
			strerror(errno)));
		return False;
	}

	switch (capability) {
		case KERNEL_OPLOCK_CAPABILITY:
#ifdef CAP_NETWORK_MGT
			/* IRIX has CAP_NETWORK_MGT for oplocks. */
			cap_vals[num_cap_vals++] = CAP_NETWORK_MGT;
#endif
			break;
		case DMAPI_ACCESS_CAPABILITY:
#ifdef CAP_DEVICE_MGT
			/* IRIX has CAP_DEVICE_MGT for DMAPI access. */
			cap_vals[num_cap_vals++] = CAP_DEVICE_MGT;
#elif CAP_MKNOD
			/* Linux has CAP_MKNOD for DMAPI access. */
			cap_vals[num_cap_vals++] = CAP_MKNOD;
#endif
			break;
		case LEASE_CAPABILITY:
#ifdef CAP_LEASE
			cap_vals[num_cap_vals++] = CAP_LEASE;
#endif
			break;
		case DAC_OVERRIDE_CAPABILITY:
#ifdef CAP_DAC_OVERRIDE
			cap_vals[num_cap_vals++] = CAP_DAC_OVERRIDE;
#endif
	}

	SMB_ASSERT(num_cap_vals <= ARRAY_SIZE(cap_vals));

	if (num_cap_vals == 0) {
		cap_free(cap);
		return True;
	}

	cap_set_flag(cap, CAP_EFFECTIVE, num_cap_vals, cap_vals,
		enable ? CAP_SET : CAP_CLEAR);

	/* We never want to pass capabilities down to our children, so make
	 * sure they are not inherited.
	 */
	cap_set_flag(cap, CAP_INHERITABLE, num_cap_vals, cap_vals, CAP_CLEAR);

	if (cap_set_proc(cap) == -1) {
		DEBUG(0, ("set_process_capability: cap_set_proc failed: %s\n",
			strerror(errno)));
		cap_free(cap);
		return False;
	}

	cap_free(cap);
	return True;
}