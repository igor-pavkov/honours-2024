static bool duplicate_flag(RFlagItem *flag, void *u) {
	struct duplicate_flag_t *user = (struct duplicate_flag_t *)u;
	/* filter per flag spaces */
	if (r_str_glob (flag->name, user->word)) {
		RFlagItem *cloned_item = r_flag_item_clone (flag);
		if (!cloned_item) {
			return false;
		}
		r_list_append (user->ret, cloned_item);
	}
	return true;
}