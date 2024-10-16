static void selinux_bpf_prog_free(struct bpf_prog_aux *aux)
{
	struct bpf_security_struct *bpfsec = aux->security;

	aux->security = NULL;
	kfree(bpfsec);
}