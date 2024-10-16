static bool tomoyo_check_mount_acl(struct tomoyo_request_info *r,
				   const struct tomoyo_acl_info *ptr)
{
	const struct tomoyo_mount_acl *acl =
		container_of(ptr, typeof(*acl), head);
	return tomoyo_compare_number_union(r->param.mount.flags, &acl->flags) &&
		tomoyo_compare_name_union(r->param.mount.type, &acl->fs_type) &&
		tomoyo_compare_name_union(r->param.mount.dir, &acl->dir_name) &&
		(!r->param.mount.need_dev ||
		 tomoyo_compare_name_union(r->param.mount.dev, &acl->dev_name));
}