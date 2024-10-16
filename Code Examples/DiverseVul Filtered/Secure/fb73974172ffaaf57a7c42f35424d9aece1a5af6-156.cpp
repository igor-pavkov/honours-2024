static int selinux_move_mount(const struct path *from_path,
			      const struct path *to_path)
{
	const struct cred *cred = current_cred();

	return path_has_perm(cred, to_path, FILE__MOUNTON);
}