static int net_assign_generic(struct net *net, unsigned int id, void *data)
{
	struct net_generic *ng, *old_ng;

	BUG_ON(!mutex_is_locked(&net_mutex));
	BUG_ON(id < MIN_PERNET_OPS_ID);

	old_ng = rcu_dereference_protected(net->gen,
					   lockdep_is_held(&net_mutex));
	if (old_ng->s.len > id) {
		old_ng->ptr[id] = data;
		return 0;
	}

	ng = net_alloc_generic();
	if (ng == NULL)
		return -ENOMEM;

	/*
	 * Some synchronisation notes:
	 *
	 * The net_generic explores the net->gen array inside rcu
	 * read section. Besides once set the net->gen->ptr[x]
	 * pointer never changes (see rules in netns/generic.h).
	 *
	 * That said, we simply duplicate this array and schedule
	 * the old copy for kfree after a grace period.
	 */

	memcpy(&ng->ptr[MIN_PERNET_OPS_ID], &old_ng->ptr[MIN_PERNET_OPS_ID],
	       (old_ng->s.len - MIN_PERNET_OPS_ID) * sizeof(void *));
	ng->ptr[id] = data;

	rcu_assign_pointer(net->gen, ng);
	kfree_rcu(old_ng, s.rcu);
	return 0;
}