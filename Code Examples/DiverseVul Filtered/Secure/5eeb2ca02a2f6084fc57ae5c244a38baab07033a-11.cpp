static int binder_inc_node_nilocked(struct binder_node *node, int strong,
				    int internal,
				    struct list_head *target_list)
{
	struct binder_proc *proc = node->proc;

	assert_spin_locked(&node->lock);
	if (proc)
		assert_spin_locked(&proc->inner_lock);
	if (strong) {
		if (internal) {
			if (target_list == NULL &&
			    node->internal_strong_refs == 0 &&
			    !(node->proc &&
			      node == node->proc->context->binder_context_mgr_node &&
			      node->has_strong_ref)) {
				pr_err("invalid inc strong node for %d\n",
					node->debug_id);
				return -EINVAL;
			}
			node->internal_strong_refs++;
		} else
			node->local_strong_refs++;
		if (!node->has_strong_ref && target_list) {
			binder_dequeue_work_ilocked(&node->work);
			/*
			 * Note: this function is the only place where we queue
			 * directly to a thread->todo without using the
			 * corresponding binder_enqueue_thread_work() helper
			 * functions; in this case it's ok to not set the
			 * process_todo flag, since we know this node work will
			 * always be followed by other work that starts queue
			 * processing: in case of synchronous transactions, a
			 * BR_REPLY or BR_ERROR; in case of oneway
			 * transactions, a BR_TRANSACTION_COMPLETE.
			 */
			binder_enqueue_work_ilocked(&node->work, target_list);
		}
	} else {
		if (!internal)
			node->local_weak_refs++;
		if (!node->has_weak_ref && list_empty(&node->work.entry)) {
			if (target_list == NULL) {
				pr_err("invalid inc weak node for %d\n",
					node->debug_id);
				return -EINVAL;
			}
			/*
			 * See comment above
			 */
			binder_enqueue_work_ilocked(&node->work, target_list);
		}
	}
	return 0;
}