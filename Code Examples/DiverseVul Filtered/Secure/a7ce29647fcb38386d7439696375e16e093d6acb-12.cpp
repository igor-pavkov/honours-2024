R_API bool r_anal_var_check_name(const char *name) {
	return !isdigit ((unsigned char)*name) && strcspn (name, "., =/");
}