R_API void r_egg_syscall(REgg *egg, const char *arg, ...) {
	RSyscallItem *item = r_syscall_get (egg->syscall,
		r_syscall_get_num (egg->syscall, arg), -1);
	if (!strcmp (arg, "close")) {
		//egg->remit->syscall_args ();
	}
	if (!item) {
		return;
	}
	egg->remit->syscall (egg, item->num);
}