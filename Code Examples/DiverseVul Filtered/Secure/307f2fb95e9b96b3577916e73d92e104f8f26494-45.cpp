static struct fib6_node * fib6_add_1(struct fib6_node *root, void *addr,
				     int addrlen, int plen,
				     int offset, int allow_create,
				     int replace_required)
{
	struct fib6_node *fn, *in, *ln;
	struct fib6_node *pn = NULL;
	struct rt6key *key;
	int	bit;
	__be32	dir = 0;
	__u32	sernum = fib6_new_sernum();

	RT6_TRACE("fib6_add_1\n");

	/* insert node in tree */

	fn = root;

	do {
		key = (struct rt6key *)((u8 *)fn->leaf + offset);

		/*
		 *	Prefix match
		 */
		if (plen < fn->fn_bit ||
		    !ipv6_prefix_equal(&key->addr, addr, fn->fn_bit)) {
			if (!allow_create) {
				if (replace_required) {
					pr_warn("Can't replace route, no match found\n");
					return ERR_PTR(-ENOENT);
				}
				pr_warn("NLM_F_CREATE should be set when creating new route\n");
			}
			goto insert_above;
		}

		/*
		 *	Exact match ?
		 */

		if (plen == fn->fn_bit) {
			/* clean up an intermediate node */
			if (!(fn->fn_flags & RTN_RTINFO)) {
				rt6_release(fn->leaf);
				fn->leaf = NULL;
			}

			fn->fn_sernum = sernum;

			return fn;
		}

		/*
		 *	We have more bits to go
		 */

		/* Try to walk down on tree. */
		fn->fn_sernum = sernum;
		dir = addr_bit_set(addr, fn->fn_bit);
		pn = fn;
		fn = dir ? fn->right: fn->left;
	} while (fn);

	if (!allow_create) {
		/* We should not create new node because
		 * NLM_F_REPLACE was specified without NLM_F_CREATE
		 * I assume it is safe to require NLM_F_CREATE when
		 * REPLACE flag is used! Later we may want to remove the
		 * check for replace_required, because according
		 * to netlink specification, NLM_F_CREATE
		 * MUST be specified if new route is created.
		 * That would keep IPv6 consistent with IPv4
		 */
		if (replace_required) {
			pr_warn("Can't replace route, no match found\n");
			return ERR_PTR(-ENOENT);
		}
		pr_warn("NLM_F_CREATE should be set when creating new route\n");
	}
	/*
	 *	We walked to the bottom of tree.
	 *	Create new leaf node without children.
	 */

	ln = node_alloc();

	if (!ln)
		return ERR_PTR(-ENOMEM);
	ln->fn_bit = plen;

	ln->parent = pn;
	ln->fn_sernum = sernum;

	if (dir)
		pn->right = ln;
	else
		pn->left  = ln;

	return ln;


insert_above:
	/*
	 * split since we don't have a common prefix anymore or
	 * we have a less significant route.
	 * we've to insert an intermediate node on the list
	 * this new node will point to the one we need to create
	 * and the current
	 */

	pn = fn->parent;

	/* find 1st bit in difference between the 2 addrs.

	   See comment in __ipv6_addr_diff: bit may be an invalid value,
	   but if it is >= plen, the value is ignored in any case.
	 */

	bit = __ipv6_addr_diff(addr, &key->addr, addrlen);

	/*
	 *		(intermediate)[in]
	 *	          /	   \
	 *	(new leaf node)[ln] (old node)[fn]
	 */
	if (plen > bit) {
		in = node_alloc();
		ln = node_alloc();

		if (!in || !ln) {
			if (in)
				node_free(in);
			if (ln)
				node_free(ln);
			return ERR_PTR(-ENOMEM);
		}

		/*
		 * new intermediate node.
		 * RTN_RTINFO will
		 * be off since that an address that chooses one of
		 * the branches would not match less specific routes
		 * in the other branch
		 */

		in->fn_bit = bit;

		in->parent = pn;
		in->leaf = fn->leaf;
		atomic_inc(&in->leaf->rt6i_ref);

		in->fn_sernum = sernum;

		/* update parent pointer */
		if (dir)
			pn->right = in;
		else
			pn->left  = in;

		ln->fn_bit = plen;

		ln->parent = in;
		fn->parent = in;

		ln->fn_sernum = sernum;

		if (addr_bit_set(addr, bit)) {
			in->right = ln;
			in->left  = fn;
		} else {
			in->left  = ln;
			in->right = fn;
		}
	} else { /* plen <= bit */

		/*
		 *		(new leaf node)[ln]
		 *	          /	   \
		 *	     (old node)[fn] NULL
		 */

		ln = node_alloc();

		if (!ln)
			return ERR_PTR(-ENOMEM);

		ln->fn_bit = plen;

		ln->parent = pn;

		ln->fn_sernum = sernum;

		if (dir)
			pn->right = ln;
		else
			pn->left  = ln;

		if (addr_bit_set(&key->addr, plen))
			ln->right = fn;
		else
			ln->left  = fn;

		fn->parent = ln;
	}
	return ln;
}