static bool torture_winbind_struct_list_users(struct torture_context *torture)
{
	char **users;
	uint32_t count;
	bool ok;

	torture_comment(torture, "Running WINBINDD_LIST_USERS (struct based)\n");

	ok = get_user_list(torture, &users);
	torture_assert(torture, ok, "failed to get user list");

	for (count = 0; users[count]; count++) { }

	torture_comment(torture, "got %d users\n", count);

	return true;
}