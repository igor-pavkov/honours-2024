static bool torture_winbind_struct_list_groups(struct torture_context *torture)
{
	char **groups;
	uint32_t count;
	bool ok;

	torture_comment(torture, "Running WINBINDD_LIST_GROUPS (struct based)\n");

	ok = get_group_list(torture, &count, &groups);
	torture_assert(torture, ok, "failed to get group list");

	torture_comment(torture, "got %d groups\n", count);

	return true;
}