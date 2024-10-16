int csync_copy_file(int fd_in, int fd_out)
{
	char buffer[512];
	int read_len = read(fd_in, buffer, 512);

	while (read_len > 0) {
		int write_len = 0;

		while (write_len < read_len) {
			int rc = write(fd_out, buffer + write_len,
				       read_len - write_len);
			if (rc == -1) {
				close(fd_in);
				close(fd_out);
				//TODO verify return code.
				return errno;
			}
			write_len += rc;
		}
		read_len = read(fd_in, buffer, 512);
	}
	close(fd_in);
	close(fd_out);
	return 0;
}