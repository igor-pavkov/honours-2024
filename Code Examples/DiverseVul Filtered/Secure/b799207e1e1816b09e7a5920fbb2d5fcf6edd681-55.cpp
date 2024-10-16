static void release_maps(struct bpf_verifier_env *env)
{
	int i;

	if (env->prog->aux->cgroup_storage)
		bpf_cgroup_storage_release(env->prog,
					   env->prog->aux->cgroup_storage);

	for (i = 0; i < env->used_map_cnt; i++)
		bpf_map_put(env->used_maps[i]);
}