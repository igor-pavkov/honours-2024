static int check_ctx_access(struct verifier_env *env, int off, int size,
			    enum bpf_access_type t)
{
	if (env->prog->aux->ops->is_valid_access &&
	    env->prog->aux->ops->is_valid_access(off, size, t))
		return 0;

	verbose("invalid bpf_context access off=%d size=%d\n", off, size);
	return -EACCES;
}