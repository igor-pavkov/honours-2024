int tomoyo_write_mount(char *data, struct tomoyo_domain_info *domain,
		       const bool is_delete)
{
	struct tomoyo_mount_acl e = { .head.type = TOMOYO_TYPE_MOUNT_ACL };
	int error = is_delete ? -ENOENT : -ENOMEM;
	char *w[4];
	if (!tomoyo_tokenize(data, w, sizeof(w)) || !w[3][0])
		return -EINVAL;
	if (!tomoyo_parse_name_union(w[0], &e.dev_name) ||
	    !tomoyo_parse_name_union(w[1], &e.dir_name) ||
	    !tomoyo_parse_name_union(w[2], &e.fs_type) ||
	    !tomoyo_parse_number_union(w[3], &e.flags))
		goto out;
	error = tomoyo_update_domain(&e.head, sizeof(e), is_delete, domain,
				     tomoyo_same_mount_acl, NULL);
 out:
	tomoyo_put_name_union(&e.dev_name);
	tomoyo_put_name_union(&e.dir_name);
	tomoyo_put_name_union(&e.fs_type);
	tomoyo_put_number_union(&e.flags);
	return error;
}