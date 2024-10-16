static void __fanout_unlink(struct sock *sk, struct packet_sock *po)
{
	struct packet_fanout *f = po->fanout;
	int i;

	spin_lock(&f->lock);
	for (i = 0; i < f->num_members; i++) {
		if (rcu_dereference_protected(f->arr[i],
					      lockdep_is_held(&f->lock)) == sk)
			break;
	}
	BUG_ON(i >= f->num_members);
	rcu_assign_pointer(f->arr[i],
			   rcu_dereference_protected(f->arr[f->num_members - 1],
						     lockdep_is_held(&f->lock)));
	f->num_members--;
	if (f->num_members == 0)
		__dev_remove_pack(&f->prot_hook);
	spin_unlock(&f->lock);
}