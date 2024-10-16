static int selinux_bpf(int cmd, union bpf_attr *attr,
				     unsigned int size)
{
	u32 sid = current_sid();
	int ret;

	switch (cmd) {
	case BPF_MAP_CREATE:
		ret = avc_has_perm(&selinux_state,
				   sid, sid, SECCLASS_BPF, BPF__MAP_CREATE,
				   NULL);
		break;
	case BPF_PROG_LOAD:
		ret = avc_has_perm(&selinux_state,
				   sid, sid, SECCLASS_BPF, BPF__PROG_LOAD,
				   NULL);
		break;
	default:
		ret = 0;
		break;
	}

	return ret;
}