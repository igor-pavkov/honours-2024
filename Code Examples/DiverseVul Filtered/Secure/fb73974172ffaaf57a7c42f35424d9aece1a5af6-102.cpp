static void selinux_bpf_map_free(struct bpf_map *map)
{
	struct bpf_security_struct *bpfsec = map->security;

	map->security = NULL;
	kfree(bpfsec);
}