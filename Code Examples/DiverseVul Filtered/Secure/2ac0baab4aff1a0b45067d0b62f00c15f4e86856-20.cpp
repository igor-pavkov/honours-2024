static char *sqfs_basename(char *path)
{
	char *fname;

	fname = path + strlen(path) - 1;
	while (fname >= path) {
		if (*fname == '/') {
			fname++;
			break;
		}

		fname--;
	}

	return fname;
}