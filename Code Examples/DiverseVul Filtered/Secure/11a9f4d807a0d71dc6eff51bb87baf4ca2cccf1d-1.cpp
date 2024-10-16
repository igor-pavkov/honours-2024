char *expand_user_path(const char *path, int real_home)
{
	struct strbuf user_path = STRBUF_INIT;
	const char *to_copy = path;

	if (path == NULL)
		goto return_null;
	if (path[0] == '~') {
		const char *first_slash = strchrnul(path, '/');
		const char *username = path + 1;
		size_t username_len = first_slash - username;
		if (username_len == 0) {
			const char *home = getenv("HOME");
			if (!home)
				goto return_null;
			if (real_home)
				strbuf_addstr(&user_path, real_path(home));
			else
				strbuf_addstr(&user_path, home);
#ifdef GIT_WINDOWS_NATIVE
			convert_slashes(user_path.buf);
#endif
		} else {
			struct passwd *pw = getpw_str(username, username_len);
			if (!pw)
				goto return_null;
			strbuf_addstr(&user_path, pw->pw_dir);
		}
		to_copy = first_slash;
	}
	strbuf_addstr(&user_path, to_copy);
	return strbuf_detach(&user_path, NULL);
return_null:
	strbuf_release(&user_path);
	return NULL;
}