int csync_file_backup(const char *filepath)
{
	static char error_buffer[1024];
	const struct csync_group *g = NULL;
	struct stat buf;
	int rc;
	/* ==============================================================================================
	 * As of now, filepath may only contain prefixes but we may need to resolve other
	 * dynamic references like environment variables, symbolic links, etc in future
	 * if we plan to support those in later releases
	 *==============================================================================================*/
	const char *filename = prefixsubst(filepath);
	int filename_len = strlen(filename);	//can filename be null?

	while ((g = csync_find_next(g, filepath))) {
		if (g->backup_directory && g->backup_generations > 1) {
			unsigned int bak_dir_len = strlen(g->backup_directory);
			char backup_filename[bak_dir_len + filename_len + 12];
			char backup_otherfilename[bak_dir_len + filename_len + 12];
			int fd_in, fd_out;
			unsigned int i, lastSlash = 0;
			mode_t mode;

			csync_debug(1, "backup %s for group %s\n", filename, g->gname);

			if (stat(g->backup_directory, &buf) != 0) {
				csync_debug(3, "backup directory configured is not present, so creating it");
				if (mkpath(g->backup_directory, 0700)) {
					csync_debug(1,
						    "ERROR : unable to create backup directory %s ; backup failed \n",
						    g->backup_directory);
					return 1;
				}
			} else if (!S_ISDIR(buf.st_mode)) {
				csync_debug(1,
					    "ERROR : location configured for backup %s is not a directory; backup failed \n",
					    g->backup_directory);
				return 1;
			}
			// Skip generation of directories
			rc = stat(filename, &buf);
			if (S_ISDIR(buf.st_mode)) {
				csync_debug(1, "directory. Skip generation \n");
				return 0;
			}

			fd_in = open(filename, O_RDONLY);
			if (fd_in < 0)
				return 0;

			memcpy(backup_filename, g->backup_directory, bak_dir_len);
			backup_filename[bak_dir_len] = 0;
			mode = 0700;

			/* open coded strrchr?? why? */
			for (i = filename_len; i > 0; i--)
				if (filename[i] == '/') {
					lastSlash = i;
					break;
				}

			for (i = 0; i < filename_len; i++) {
				// Create directories in filename
				// TODO: Get the mode from the orig. dir
				if (filename[i] == '/' && i <= lastSlash) {

					backup_filename[bak_dir_len + i] = 0;

					csync_debug(1, "mkdir %s \n", backup_filename);

					mkdir(backup_filename, mode);
					// Dont check the empty string.
					if (i != 0)
						csync_setBackupFileStatus(backup_filename, bak_dir_len);

				}
				backup_filename[bak_dir_len + i] = filename[i];
			}

			backup_filename[bak_dir_len + filename_len] = 0;
			backup_filename[bak_dir_len] = '/';
			memcpy(backup_otherfilename, backup_filename, bak_dir_len + filename_len);

			for (i = g->backup_generations - 1; i; i--) {

				if (i != 1)
					snprintf(backup_filename + bak_dir_len + filename_len, 12, ".%u", i - 1);
				backup_filename[bak_dir_len + filename_len] = '\0';
				snprintf(backup_otherfilename + bak_dir_len + filename_len, 12, ".%u", i);

				rc = rename(backup_filename, backup_otherfilename);
				csync_debug(1,
					    "renaming backup files '%s' to '%s'. rc = %d\n",
					    backup_filename, backup_otherfilename, rc);
			}

			/* strcpy(backup_filename+bak_dir_len+filename_len, ""); */

			fd_out = open(backup_filename, O_WRONLY | O_CREAT, 0600);

			if (fd_out < 0) {
				snprintf(error_buffer, 1024, "Open error while backing up '%s': %s\n", filename, strerror(errno));
				cmd_error = error_buffer;
				close(fd_in);
				return 1;
			}

			csync_debug(1, "Copying data from %s to backup file %s \n", filename, backup_filename);

			rc = csync_copy_file(fd_in, fd_out);
			if (rc != 0) {
				csync_debug(1, "csync_backup error 2\n");

				snprintf(error_buffer, 1024, "Write error while backing up '%s': %s\n", filename, strerror(errno));

				cmd_error = error_buffer;
				// TODO verify file disapeared ?
				//
				// return 1;
			}
			csync_setBackupFileStatus(backup_filename, bak_dir_len);
		}
	}
	return 0;
}