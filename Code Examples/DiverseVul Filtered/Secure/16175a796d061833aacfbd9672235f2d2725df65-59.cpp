static int init_rmode(struct kvm *kvm)
{
	if (!init_rmode_tss(kvm))
		return 0;
	if (!init_rmode_identity_map(kvm))
		return 0;
	return 1;
}