static u32 bpf_map_fmode_to_av(fmode_t fmode)
{
	u32 av = 0;

	if (fmode & FMODE_READ)
		av |= BPF__MAP_READ;
	if (fmode & FMODE_WRITE)
		av |= BPF__MAP_WRITE;
	return av;
}