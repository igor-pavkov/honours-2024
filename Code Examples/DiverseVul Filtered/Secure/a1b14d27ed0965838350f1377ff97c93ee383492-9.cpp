static struct verifier_state *push_stack(struct verifier_env *env, int insn_idx,
					 int prev_insn_idx)
{
	struct verifier_stack_elem *elem;

	elem = kmalloc(sizeof(struct verifier_stack_elem), GFP_KERNEL);
	if (!elem)
		goto err;

	memcpy(&elem->st, &env->cur_state, sizeof(env->cur_state));
	elem->insn_idx = insn_idx;
	elem->prev_insn_idx = prev_insn_idx;
	elem->next = env->head;
	env->head = elem;
	env->stack_size++;
	if (env->stack_size > 1024) {
		verbose("BPF program is too complex\n");
		goto err;
	}
	return &elem->st;
err:
	/* pop all elements and return */
	while (pop_stack(env, NULL) >= 0);
	return NULL;
}