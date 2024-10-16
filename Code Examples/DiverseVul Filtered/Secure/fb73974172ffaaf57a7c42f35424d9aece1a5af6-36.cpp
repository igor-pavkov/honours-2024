static int selinux_bpf_prog(struct bpf_prog *prog)
{
	u32 sid = current_sid();
	struct bpf_security_struct *bpfsec;

	bpfsec = prog->aux->security;
	return avc_has_perm(&selinux_state,
			    sid, bpfsec->sid, SECCLASS_BPF,
			    BPF__PROG_RUN, NULL);
}