static int selinux_bpf_prog_alloc(struct bpf_prog_aux *aux)
{
	struct bpf_security_struct *bpfsec;

	bpfsec = kzalloc(sizeof(*bpfsec), GFP_KERNEL);
	if (!bpfsec)
		return -ENOMEM;

	bpfsec->sid = current_sid();
	aux->security = bpfsec;

	return 0;
}