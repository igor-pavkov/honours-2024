static int netns_list(int argc, char **argv)
{
	struct dirent *entry;
	DIR *dir;
	int id;

	dir = opendir(NETNS_RUN_DIR);
	if (!dir)
		return 0;

	new_json_obj(json);
	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;

		open_json_object(NULL);
		print_string(PRINT_ANY, "name",
			     "%s", entry->d_name);
		if (ipnetns_have_nsid()) {
			id = get_netnsid_from_name(entry->d_name);
			if (id >= 0)
				print_uint(PRINT_ANY, "id",
					   " (id: %d)", id);
		}
		print_string(PRINT_FP, NULL, "\n", NULL);
		close_json_object();
	}
	closedir(dir);
	delete_json_obj();
	return 0;
}