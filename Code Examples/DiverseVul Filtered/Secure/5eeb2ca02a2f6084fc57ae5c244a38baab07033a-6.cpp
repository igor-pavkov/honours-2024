static void binder_deferred_release(struct binder_proc *proc)
{
	struct binder_context *context = proc->context;
	struct rb_node *n;
	int threads, nodes, incoming_refs, outgoing_refs, active_transactions;

	BUG_ON(proc->files);

	mutex_lock(&binder_procs_lock);
	hlist_del(&proc->proc_node);
	mutex_unlock(&binder_procs_lock);

	mutex_lock(&context->context_mgr_node_lock);
	if (context->binder_context_mgr_node &&
	    context->binder_context_mgr_node->proc == proc) {
		binder_debug(BINDER_DEBUG_DEAD_BINDER,
			     "%s: %d context_mgr_node gone\n",
			     __func__, proc->pid);
		context->binder_context_mgr_node = NULL;
	}
	mutex_unlock(&context->context_mgr_node_lock);
	binder_inner_proc_lock(proc);
	/*
	 * Make sure proc stays alive after we
	 * remove all the threads
	 */
	proc->tmp_ref++;

	proc->is_dead = true;
	threads = 0;
	active_transactions = 0;
	while ((n = rb_first(&proc->threads))) {
		struct binder_thread *thread;

		thread = rb_entry(n, struct binder_thread, rb_node);
		binder_inner_proc_unlock(proc);
		threads++;
		active_transactions += binder_thread_release(proc, thread);
		binder_inner_proc_lock(proc);
	}

	nodes = 0;
	incoming_refs = 0;
	while ((n = rb_first(&proc->nodes))) {
		struct binder_node *node;

		node = rb_entry(n, struct binder_node, rb_node);
		nodes++;
		/*
		 * take a temporary ref on the node before
		 * calling binder_node_release() which will either
		 * kfree() the node or call binder_put_node()
		 */
		binder_inc_node_tmpref_ilocked(node);
		rb_erase(&node->rb_node, &proc->nodes);
		binder_inner_proc_unlock(proc);
		incoming_refs = binder_node_release(node, incoming_refs);
		binder_inner_proc_lock(proc);
	}
	binder_inner_proc_unlock(proc);

	outgoing_refs = 0;
	binder_proc_lock(proc);
	while ((n = rb_first(&proc->refs_by_desc))) {
		struct binder_ref *ref;

		ref = rb_entry(n, struct binder_ref, rb_node_desc);
		outgoing_refs++;
		binder_cleanup_ref_olocked(ref);
		binder_proc_unlock(proc);
		binder_free_ref(ref);
		binder_proc_lock(proc);
	}
	binder_proc_unlock(proc);

	binder_release_work(proc, &proc->todo);
	binder_release_work(proc, &proc->delivered_death);

	binder_debug(BINDER_DEBUG_OPEN_CLOSE,
		     "%s: %d threads %d, nodes %d (ref %d), refs %d, active transactions %d\n",
		     __func__, proc->pid, threads, nodes, incoming_refs,
		     outgoing_refs, active_transactions);

	binder_proc_dec_tmpref(proc);
}