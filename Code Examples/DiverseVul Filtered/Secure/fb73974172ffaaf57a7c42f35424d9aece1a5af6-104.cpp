static int bpf_fd_pass(struct file *file, u32 sid)
{
	struct bpf_security_struct *bpfsec;
	struct bpf_prog *prog;
	struct bpf_map *map;
	int ret;

	if (file->f_op == &bpf_map_fops) {
		map = file->private_data;
		bpfsec = map->security;
		ret = avc_has_perm(&selinux_state,
				   sid, bpfsec->sid, SECCLASS_BPF,
				   bpf_map_fmode_to_av(file->f_mode), NULL);
		if (ret)
			return ret;
	} else if (file->f_op == &bpf_prog_fops) {
		prog = file->private_data;
		bpfsec = prog->aux->security;
		ret = avc_has_perm(&selinux_state,
				   sid, bpfsec->sid, SECCLASS_BPF,
				   BPF__PROG_RUN, NULL);
		if (ret)
			return ret;
	}
	return 0;
}