static void release_maps(struct verifier_env *env)
{
	int i;

	for (i = 0; i < env->used_map_cnt; i++)
		bpf_map_put(env->used_maps[i]);
}