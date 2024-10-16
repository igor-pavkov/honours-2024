static int selinux_bpf_map_alloc(struct bpf_map *map)
{
	struct bpf_security_struct *bpfsec;

	bpfsec = kzalloc(sizeof(*bpfsec), GFP_KERNEL);
	if (!bpfsec)
		return -ENOMEM;

	bpfsec->sid = current_sid();
	map->security = bpfsec;

	return 0;
}