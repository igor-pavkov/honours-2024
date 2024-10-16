static char *get_nic_if_avail(int fd, struct alloted_s *names, int pid,
			      char *intype, char *br, int allowed, char **cnic)
{
	int ret;
	size_t slen;
	char *newline, *owner;
	char nicname[IFNAMSIZ];
	struct stat sb;
	struct alloted_s *n;
	int count = 0;
	char *buf = NULL;

	for (n = names; n != NULL; n = n->next)
		cull_entries(fd, n->name, intype, br, NULL, NULL);

	if (allowed == 0)
		return NULL;

	owner = names->name;

	ret = fstat(fd, &sb);
	if (ret < 0) {
		usernic_error("Failed to fstat: %s\n", strerror(errno));
		return NULL;
	}

	if (sb.st_size > 0) {
		buf = lxc_strmmap(NULL, sb.st_size, PROT_READ | PROT_WRITE,
				  MAP_SHARED, fd, 0);
		if (buf == MAP_FAILED) {
			usernic_error("Failed to establish shared memory "
				      "mapping: %s\n", strerror(errno));
			return NULL;
		}

		owner = NULL;
		for (n = names; n != NULL; n = n->next) {
			count = count_entries(buf, sb.st_size, n->name, intype, br);
			if (count >= n->allowed)
				continue;

			owner = n->name;
			break;
		}

		lxc_strmunmap(buf, sb.st_size);
	}

	if (owner == NULL)
		return NULL;

	ret = snprintf(nicname, sizeof(nicname), "vethXXXXXX");
	if (ret < 0 || (size_t)ret >= sizeof(nicname))
		return NULL;

	if (!lxc_mkifname(nicname))
		return NULL;

	ret = create_nic(nicname, br, pid, cnic);
	if (ret < 0) {
		usernic_error("%s", "Failed to create new nic\n");
		return NULL;
	}

	/* strlen(owner)
	 * +
	 * " "
	 * +
	 * strlen(intype)
	 * +
	 * " "
	 * +
	 * strlen(br)
	 * +
	 * " "
	 * +
	 * strlen(nicname)
	 * +
	 * \n
	 * +
	 * \0
	 */
	slen = strlen(owner) + strlen(intype) + strlen(br) + strlen(nicname) + 4;
	newline = malloc(slen + 1);
	if (!newline) {
		free(newline);
		usernic_error("Failed allocate memory: %s\n", strerror(errno));
		return NULL;
	}

	ret = snprintf(newline, slen + 1, "%s %s %s %s\n", owner, intype, br, nicname);
	if (ret < 0 || (size_t)ret >= (slen + 1)) {
		if (lxc_netdev_delete_by_name(nicname) != 0)
			usernic_error("Error unlinking %s\n", nicname);
		free(newline);
		return NULL;
	}

	/* Note that the file needs to be truncated to the size **without** the
	 * \0 byte! Files are not \0-terminated!
	 */
	ret = ftruncate(fd, sb.st_size + slen);
	if (ret < 0)
		usernic_error("Failed to truncate file: %s\n", strerror(errno));

	buf = lxc_strmmap(NULL, sb.st_size + slen, PROT_READ | PROT_WRITE,
			  MAP_SHARED, fd, 0);
	if (buf == MAP_FAILED) {
		usernic_error("Failed to establish shared memory mapping: %s\n",
			      strerror(errno));
		if (lxc_netdev_delete_by_name(nicname) != 0)
			usernic_error("Error unlinking %s\n", nicname);
		free(newline);
		return NULL;
	}

	/* Note that the memory needs to be moved in the buffer **without** the
	 * \0 byte! Files are not \0-terminated!
	 */
	memmove(buf + sb.st_size, newline, slen);
	free(newline);
	lxc_strmunmap(buf, sb.st_size + slen);

	return strdup(nicname);
}