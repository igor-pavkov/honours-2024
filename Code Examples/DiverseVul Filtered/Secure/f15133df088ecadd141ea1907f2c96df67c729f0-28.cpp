static int path_lookupat(int dfd, const struct filename *name,
				unsigned int flags, struct nameidata *nd)
{
	struct path path;
	int err;

	/*
	 * Path walking is largely split up into 2 different synchronisation
	 * schemes, rcu-walk and ref-walk (explained in
	 * Documentation/filesystems/path-lookup.txt). These share much of the
	 * path walk code, but some things particularly setup, cleanup, and
	 * following mounts are sufficiently divergent that functions are
	 * duplicated. Typically there is a function foo(), and its RCU
	 * analogue, foo_rcu().
	 *
	 * -ECHILD is the error number of choice (just to avoid clashes) that
	 * is returned if some aspect of an rcu-walk fails. Such an error must
	 * be handled by restarting a traditional ref-walk (which will always
	 * be able to complete).
	 */
	err = path_init(dfd, name, flags, nd);
	if (!err && !(flags & LOOKUP_PARENT)) {
		err = lookup_last(nd, &path);
		while (err > 0) {
			void *cookie;
			struct path link = path;
			err = may_follow_link(&link, nd);
			if (unlikely(err))
				break;
			nd->flags |= LOOKUP_PARENT;
			err = follow_link(&link, nd, &cookie);
			if (err)
				break;
			err = lookup_last(nd, &path);
			put_link(nd, &link, cookie);
		}
	}

	if (!err)
		err = complete_walk(nd);

	if (!err && nd->flags & LOOKUP_DIRECTORY) {
		if (!d_can_lookup(nd->path.dentry)) {
			path_put(&nd->path);
			err = -ENOTDIR;
		}
	}

	path_cleanup(nd);
	return err;
}