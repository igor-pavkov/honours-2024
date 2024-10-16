static int push_check(int argc, const char **argv, const char *prefix)
{
	struct remote *remote;
	const char *superproject_head;
	char *head;
	int detached_head = 0;
	struct object_id head_oid;

	if (argc < 3)
		die("submodule--helper push-check requires at least 2 arguments");

	/*
	 * superproject's resolved head ref.
	 * if HEAD then the superproject is in a detached head state, otherwise
	 * it will be the resolved head ref.
	 */
	superproject_head = argv[1];
	argv++;
	argc--;
	/* Get the submodule's head ref and determine if it is detached */
	head = resolve_refdup("HEAD", 0, head_oid.hash, NULL);
	if (!head)
		die(_("Failed to resolve HEAD as a valid ref."));
	if (!strcmp(head, "HEAD"))
		detached_head = 1;

	/*
	 * The remote must be configured.
	 * This is to avoid pushing to the exact same URL as the parent.
	 */
	remote = pushremote_get(argv[1]);
	if (!remote || remote->origin == REMOTE_UNCONFIGURED)
		die("remote '%s' not configured", argv[1]);

	/* Check the refspec */
	if (argc > 2) {
		int i, refspec_nr = argc - 2;
		struct ref *local_refs = get_local_heads();
		struct refspec *refspec = parse_push_refspec(refspec_nr,
							     argv + 2);

		for (i = 0; i < refspec_nr; i++) {
			struct refspec *rs = refspec + i;

			if (rs->pattern || rs->matching)
				continue;

			/* LHS must match a single ref */
			switch (count_refspec_match(rs->src, local_refs, NULL)) {
			case 1:
				break;
			case 0:
				/*
				 * If LHS matches 'HEAD' then we need to ensure
				 * that it matches the same named branch
				 * checked out in the superproject.
				 */
				if (!strcmp(rs->src, "HEAD")) {
					if (!detached_head &&
					    !strcmp(head, superproject_head))
						break;
					die("HEAD does not match the named branch in the superproject");
				}
			default:
				die("src refspec '%s' must name a ref",
				    rs->src);
			}
		}
		free_refspec(refspec_nr, refspec);
	}
	free(head);

	return 0;
}