static pid_t clone_newns(void *a)
{
	char buf[131072];
	char *stack = buf + (sizeof(buf) / 2 - ((size_t) buf & 15));

#ifdef __ia64__
	extern int __clone2(int (*fn)(void *),
			    void *child_stack_base, size_t stack_size,
			    int flags, void *arg, pid_t *ptid,
			    void *tls, pid_t *ctid);

	return __clone2(check_is_mount_child, stack, sizeof(buf) / 2,
			CLONE_NEWNS, a, NULL, NULL, NULL);
#else
	return clone(check_is_mount_child, stack, CLONE_NEWNS, a);
#endif
}