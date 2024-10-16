static int check_map_prog_compatibility(struct bpf_verifier_env *env,
					struct bpf_map *map,
					struct bpf_prog *prog)

{
	/* Make sure that BPF_PROG_TYPE_PERF_EVENT programs only use
	 * preallocated hash maps, since doing memory allocation
	 * in overflow_handler can crash depending on where nmi got
	 * triggered.
	 */
	if (prog->type == BPF_PROG_TYPE_PERF_EVENT) {
		if (!check_map_prealloc(map)) {
			verbose(env, "perf_event programs can only use preallocated hash map\n");
			return -EINVAL;
		}
		if (map->inner_map_meta &&
		    !check_map_prealloc(map->inner_map_meta)) {
			verbose(env, "perf_event programs can only use preallocated inner hash map\n");
			return -EINVAL;
		}
	}

	if ((bpf_prog_is_dev_bound(prog->aux) || bpf_map_is_dev_bound(map)) &&
	    !bpf_offload_prog_map_match(prog, map)) {
		verbose(env, "offload device mismatch between prog and map\n");
		return -EINVAL;
	}

	return 0;
}