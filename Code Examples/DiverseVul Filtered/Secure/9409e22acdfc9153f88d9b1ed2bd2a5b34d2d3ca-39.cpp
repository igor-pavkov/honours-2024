static int unlazy_link(struct nameidata *nd, struct path *link, unsigned seq)
{
	if (unlikely(!legitimize_path(nd, link, seq))) {
		drop_links(nd);
		nd->depth = 0;
		nd->flags &= ~LOOKUP_RCU;
		nd->path.mnt = NULL;
		nd->path.dentry = NULL;
		if (!(nd->flags & LOOKUP_ROOT))
			nd->root.mnt = NULL;
		rcu_read_unlock();
	} else if (likely(unlazy_walk(nd, NULL, 0)) == 0) {
		return 0;
	}
	path_put(link);
	return -ECHILD;
}