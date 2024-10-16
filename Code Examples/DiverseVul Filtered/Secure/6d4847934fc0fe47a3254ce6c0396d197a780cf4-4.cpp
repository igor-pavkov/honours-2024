void handle(int infd, int outfd)
{
  FILE *fp = fdopen(infd, "r");
  char *s = xgetline(fp), *ss, *esc, *path, *word[3];
  int i, fd;

  // Split line into method/path/protocol
  for (i = 0, ss = s;;) {
    word[i++] = ss;
    while (*ss && !strchr(" \r\n", *ss)) ss++;
    while (*ss && strchr(" \r\n", *ss)) *(ss++) = 0;
    if (i==3) break;
    if (!*ss) return header_time(400, "Bad Request", 0);
  }

  // Process additional http/1.1 lines
  while ((ss = xgetline(fp))) {
    i = *chomp(ss);
// TODO: any of
//User-Agent: Wget/1.20.1 (linux-gnu) - do we want to log anything?
//Accept: */* - 406 Too Snobbish
//Accept-Encoding: identity - we can gzip?
//Host: landley.net  - we could handle multiple domains?
//Connection: Keep-Alive - probably don't care

    free(ss);
    if (!i) break;
  }

  if (!strcasecmp(word[0], "get")) {
    struct stat st;
    if (*(ss = word[1])!='/') error_time(400, "Bad Request");
    while (*ss=='/') ss++;
    if (!*ss) ss = ".";
    else unescape_url(ss);

    // TODO domain.com:/path/to/blah domain2.com:/path/to/that
    if (!isunder(ss, ".") || stat(ss, &st)) error_time(404, "Not Found");
    else if (-1 == (fd = open(ss, O_RDONLY))) error_time(403, "Forbidden");
    else if (!S_ISDIR(st.st_mode)) {
      char buf[64];
file:
      header_time(200, "Ok", ss = xmprintf("Content-Type: %s\r\n"
        "Content-Length: %lld\r\nLast-Modified: %s\r\n",
        mime(ss), (long long)st.st_size, rfc1123(buf, st.st_mtime)));
      free(ss);
      xsendfile(fd, outfd);
    } else if (ss[strlen(ss)-1]!='/' && strcmp(ss, ".")) {
      header_time(302, "Found", path = xmprintf("Location: %s/\r\n", word[1]));
      free(path);
    } else {
      DIR *dd;
      struct dirent *dir;

      // Do we have an index.html?
      path = ss;
      ss = "index.html";
      path = xmprintf("%s%s", path, ss);
      if (!stat(path, &st) || !S_ISREG(st.st_mode)) i = -1;
      else if (-1 == (i = open(path, O_RDONLY))) error_time(403, "Forbidden");
      free(path);
      if (i != -1) {
        close(fd);
        fd = i;

        goto file;
      }

      // List directory contents
      header_time(200, "Ok", "Content-Type: text/html\r\n");
      dprintf(outfd, "<html><head><title>Index of %s</title></head>\n"
        "<body><h3>Index of %s</h3></body>\n", word[1], word[1]);
      for (dd = fdopendir(fd); (dir = readdir(dd));) {
        esc = escape_url(dir->d_name, "<>&\"");
        dprintf(outfd, "<a href=\"%s\">%s</a><br />\n", esc, esc);
        free(esc);
      }
      dprintf(outfd, "</body></html>\n");
    }
  } else error_time(501, "Not Implemented");
  free(s);
}