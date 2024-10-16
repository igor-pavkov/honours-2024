static void start_unregistering(struct ctl_table_header *p)
{
	/*
	 * if p->used is 0, nobody will ever touch that entry again;
	 * we'll eliminate all paths to it before dropping sysctl_lock
	 */
	if (unlikely(p->used)) {
		struct completion wait;
		init_completion(&wait);
		p->unregistering = &wait;
		spin_unlock(&sysctl_lock);
		wait_for_completion(&wait);
	} else {
		/* anything non-NULL; we'll never dereference it */
		p->unregistering = ERR_PTR(-EINVAL);
		spin_unlock(&sysctl_lock);
	}
	/*
	 * Prune dentries for unregistered sysctls: namespaced sysctls
	 * can have duplicate names and contaminate dcache very badly.
	 */
	proc_sys_prune_dcache(p);
	/*
	 * do not remove from the list until nobody holds it; walking the
	 * list in do_sysctl() relies on that.
	 */
	spin_lock(&sysctl_lock);
	erase_header(p);
}