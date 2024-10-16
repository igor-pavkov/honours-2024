static int remove_recursively(const char * path) {
  DIR *d = opendir(path);
  size_t path_len = strlen(path);
  int r = -1;
  size_t len;
  char *buf;

  if (d) {
    struct dirent *p;

    r = 0;

    while ((r==0) && (p=readdir(d))) {
      /* Skip the names "." and ".." as we don't want to recurse on them. */
      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
         continue;

      len = path_len + strlen(p->d_name) + 2;
      buf = (char *) malloc(len);

      if (buf) {
        struct stat statbuf;

        snprintf(buf, len, "%s/%s", path, p->d_name);

        if (stat(buf, &statbuf) == 0) {
          if (S_ISDIR(statbuf.st_mode))
            r = remove_recursively(buf);
          else
            r = unlink(buf);
        }

        free(buf);
      }
    }

    closedir(d);
  }

  if (r == 0)
    r = rmdir(path);

  return r;
}