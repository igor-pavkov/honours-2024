static int selinux_capable(const struct cred *cred, struct user_namespace *ns,
			   int cap, int audit)
{
	return cred_has_capability(cred, cap, audit, ns == &init_user_ns);
}