get_file(cupsd_client_t *con,		/* I  - Client connection */
         struct stat    *filestats,	/* O  - File information */
         char           *filename,	/* IO - Filename buffer */
         size_t         len)		/* I  - Buffer length */
{
  int		status;			/* Status of filesystem calls */
  char		*ptr;			/* Pointer info filename */
  size_t	plen;			/* Remaining length after pointer */
  char		language[7],		/* Language subdirectory, if any */
		dest[1024];		/* Destination name */
  int		perm_check = 1;		/* Do permissions check? */


 /*
  * Figure out the real filename...
  */

  language[0] = '\0';

  if (!strncmp(con->uri, "/ppd/", 5) && !strchr(con->uri + 5, '/'))
  {
    strlcpy(dest, con->uri + 5, sizeof(dest));
    ptr = dest + strlen(dest) - 4;

    if (ptr <= dest || strcmp(ptr, ".ppd"))
    {
      cupsdLogClient(con, CUPSD_LOG_INFO, "Disallowed path \"%s\".", con->uri);
      return (NULL);
    }

    *ptr = '\0';
    if (!cupsdFindPrinter(dest))
    {
      cupsdLogClient(con, CUPSD_LOG_INFO, "No printer \"%s\" found.", dest);
      return (NULL);
    }

    snprintf(filename, len, "%s%s", ServerRoot, con->uri);

    perm_check = 0;
  }
  else if (!strncmp(con->uri, "/icons/", 7) && !strchr(con->uri + 7, '/'))
  {
    strlcpy(dest, con->uri + 7, sizeof(dest));
    ptr = dest + strlen(dest) - 4;

    if (ptr <= dest || strcmp(ptr, ".png"))
    {
      cupsdLogClient(con, CUPSD_LOG_INFO, "Disallowed path \"%s\".", con->uri);
      return (NULL);
    }

    *ptr = '\0';
    if (!cupsdFindDest(dest))
    {
      cupsdLogClient(con, CUPSD_LOG_INFO, "No printer \"%s\" found.", dest);
      return (NULL);
    }

    snprintf(filename, len, "%s/%s.png", CacheDir, dest);
    if (access(filename, F_OK) < 0)
      snprintf(filename, len, "%s/images/generic.png", DocumentRoot);

    perm_check = 0;
  }
  else if (!strncmp(con->uri, "/rss/", 5) && !strchr(con->uri + 5, '/'))
    snprintf(filename, len, "%s/rss/%s", CacheDir, con->uri + 5);
  else if (!strcmp(con->uri, "/admin/conf/cupsd.conf"))
  {
    strlcpy(filename, ConfigurationFile, len);

    perm_check = 0;
  }
  else if (!strncmp(con->uri, "/admin/log/", 11))
  {
    if (!strncmp(con->uri + 11, "access_log", 10) && AccessLog[0] == '/')
      strlcpy(filename, AccessLog, len);
    else if (!strncmp(con->uri + 11, "error_log", 9) && ErrorLog[0] == '/')
      strlcpy(filename, ErrorLog, len);
    else if (!strncmp(con->uri + 11, "page_log", 8) && PageLog[0] == '/')
      strlcpy(filename, PageLog, len);
    else
      return (NULL);

    perm_check = 0;
  }
  else if (con->language)
  {
    snprintf(language, sizeof(language), "/%s", con->language->language);
    snprintf(filename, len, "%s%s%s", DocumentRoot, language, con->uri);
  }
  else
    snprintf(filename, len, "%s%s", DocumentRoot, con->uri);

  if ((ptr = strchr(filename, '?')) != NULL)
    *ptr = '\0';

 /*
  * Grab the status for this language; if there isn't a language-specific file
  * then fallback to the default one...
  */

  if ((status = lstat(filename, filestats)) != 0 && language[0] &&
      strncmp(con->uri, "/icons/", 7) &&
      strncmp(con->uri, "/ppd/", 5) &&
      strncmp(con->uri, "/rss/", 5) &&
      strncmp(con->uri, "/admin/conf/", 12) &&
      strncmp(con->uri, "/admin/log/", 11))
  {
   /*
    * Drop the country code...
    */

    language[3] = '\0';
    snprintf(filename, len, "%s%s%s", DocumentRoot, language, con->uri);

    if ((ptr = strchr(filename, '?')) != NULL)
      *ptr = '\0';

    if ((status = lstat(filename, filestats)) != 0)
    {
     /*
      * Drop the language prefix and try the root directory...
      */

      language[0] = '\0';
      snprintf(filename, len, "%s%s", DocumentRoot, con->uri);

      if ((ptr = strchr(filename, '?')) != NULL)
	*ptr = '\0';

      status = lstat(filename, filestats);
    }
  }

 /*
  * If we've found a symlink, 404 the sucker to avoid disclosing information.
  */

  if (!status && S_ISLNK(filestats->st_mode))
  {
    cupsdLogClient(con, CUPSD_LOG_INFO, "Symlinks such as \"%s\" are not allowed.", filename);
    return (NULL);
  }

 /*
  * Similarly, if the file/directory does not have world read permissions, do
  * not allow access...
  */

  if (!status && perm_check && !(filestats->st_mode & S_IROTH))
  {
    cupsdLogClient(con, CUPSD_LOG_INFO, "Files/directories such as \"%s\" must be world-readable.", filename);
    return (NULL);
  }

 /*
  * If we've found a directory, get the index.html file instead...
  */

  if (!status && S_ISDIR(filestats->st_mode))
  {
   /*
    * Make sure the URI ends with a slash...
    */

    if (con->uri[strlen(con->uri) - 1] != '/')
      strlcat(con->uri, "/", sizeof(con->uri));

   /*
    * Find the directory index file, trying every language...
    */

    do
    {
      if (status && language[0])
      {
       /*
        * Try a different language subset...
	*/

	if (language[3])
	  language[0] = '\0';		/* Strip country code */
	else
	  language[0] = '\0';		/* Strip language */
      }

     /*
      * Look for the index file...
      */

      snprintf(filename, len, "%s%s%s", DocumentRoot, language, con->uri);

      if ((ptr = strchr(filename, '?')) != NULL)
	*ptr = '\0';

      ptr  = filename + strlen(filename);
      plen = len - (size_t)(ptr - filename);

      strlcpy(ptr, "index.html", plen);
      status = lstat(filename, filestats);

#ifdef HAVE_JAVA
      if (status)
      {
	strlcpy(ptr, "index.class", plen);
	status = lstat(filename, filestats);
      }
#endif /* HAVE_JAVA */

#ifdef HAVE_PERL
      if (status)
      {
	strlcpy(ptr, "index.pl", plen);
	status = lstat(filename, filestats);
      }
#endif /* HAVE_PERL */

#ifdef HAVE_PHP
      if (status)
      {
	strlcpy(ptr, "index.php", plen);
	status = lstat(filename, filestats);
      }
#endif /* HAVE_PHP */

#ifdef HAVE_PYTHON
      if (status)
      {
	strlcpy(ptr, "index.pyc", plen);
	status = lstat(filename, filestats);
      }

      if (status)
      {
	strlcpy(ptr, "index.py", plen);
	status = lstat(filename, filestats);
      }
#endif /* HAVE_PYTHON */

    }
    while (status && language[0]);

   /*
    * If we've found a symlink, 404 the sucker to avoid disclosing information.
    */

    if (!status && S_ISLNK(filestats->st_mode))
    {
      cupsdLogClient(con, CUPSD_LOG_INFO, "Symlinks such as \"%s\" are not allowed.", filename);
      return (NULL);
    }

   /*
    * Similarly, if the file/directory does not have world read permissions, do
    * not allow access...
    */

    if (!status && perm_check && !(filestats->st_mode & S_IROTH))
    {
      cupsdLogClient(con, CUPSD_LOG_INFO, "Files/directories such as \"%s\" must be world-readable.", filename);
      return (NULL);
    }
  }

  cupsdLogClient(con, CUPSD_LOG_DEBUG2, "get_file: filestats=%p, filename=%p, len=" CUPS_LLFMT ", returning \"%s\".", filestats, filename, CUPS_LLCAST len, status ? "(null)" : filename);

  if (status)
    return (NULL);
  else
    return (filename);
}