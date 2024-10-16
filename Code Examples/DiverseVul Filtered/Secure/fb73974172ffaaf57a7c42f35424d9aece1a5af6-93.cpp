static int selinux_bpf_map(struct bpf_map *map, fmode_t fmode)
{
	u32 sid = current_sid();
	struct bpf_security_struct *bpfsec;

	bpfsec = map->security;
	return avc_has_perm(&selinux_state,
			    sid, bpfsec->sid, SECCLASS_BPF,
			    bpf_map_fmode_to_av(fmode), NULL);
}