static int parse_sid(struct super_block *sb, const char *s, u32 *sid)
{
	int rc = security_context_str_to_sid(&selinux_state, s,
					     sid, GFP_KERNEL);
	if (rc)
		pr_warn("SELinux: security_context_str_to_sid"
		       "(%s) failed for (dev %s, type %s) errno=%d\n",
		       s, sb->s_id, sb->s_type->name, rc);
	return rc;
}