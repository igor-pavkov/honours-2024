static void ip_del_fnhe(struct fib_nh *nh, __be32 daddr)
{
	struct fnhe_hash_bucket *hash;
	struct fib_nh_exception *fnhe, __rcu **fnhe_p;
	u32 hval = fnhe_hashfun(daddr);

	spin_lock_bh(&fnhe_lock);

	hash = rcu_dereference_protected(nh->nh_exceptions,
					 lockdep_is_held(&fnhe_lock));
	hash += hval;

	fnhe_p = &hash->chain;
	fnhe = rcu_dereference_protected(*fnhe_p, lockdep_is_held(&fnhe_lock));
	while (fnhe) {
		if (fnhe->fnhe_daddr == daddr) {
			rcu_assign_pointer(*fnhe_p, rcu_dereference_protected(
				fnhe->fnhe_next, lockdep_is_held(&fnhe_lock)));
			fnhe_flush_routes(fnhe);
			kfree_rcu(fnhe, rcu);
			break;
		}
		fnhe_p = &fnhe->fnhe_next;
		fnhe = rcu_dereference_protected(fnhe->fnhe_next,
						 lockdep_is_held(&fnhe_lock));
	}

	spin_unlock_bh(&fnhe_lock);
}