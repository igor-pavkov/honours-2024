static int create_dir_hierarchy(const char *outfile)
{
  char *tempdir;
  char *tempdir2;
  char *outdup;
  char *dirbuildup;
  int result=0;
  int error;

  outdup = strdup(outfile);
  if(!outdup)
    return -1;

  dirbuildup = malloc(sizeof(char) * strlen(outfile));
  if(!dirbuildup) {
    free(outdup);
    return -1;
  }
  dirbuildup[0] = '\0';

  tempdir = strtok(outdup, DIR_CHAR);

  while (tempdir != NULL) {
    tempdir2 = strtok(NULL, DIR_CHAR);
    /* since strtok returns a token for the last word even
       if not ending with DIR_CHAR, we need to prune it */
    if (tempdir2 != NULL) {
      if (strlen(dirbuildup) > 0)
        sprintf(dirbuildup,"%s%s%s",dirbuildup, DIR_CHAR, tempdir);
      else {
        if (0 != strncmp(outdup, DIR_CHAR, 1))
          sprintf(dirbuildup,"%s",tempdir);
        else
          sprintf(dirbuildup,"%s%s", DIR_CHAR, tempdir);
      }
      if (access(dirbuildup, F_OK) == -1) {
        result = mkdir(dirbuildup,(mode_t)0000750);
        if (-1 == result) {
          error = ERRNO;
          switch (error) {
#ifdef EACCES
          case EACCES:
            fprintf(stderr,"You don't have permission to create %s.\n",
                    dirbuildup);
            break;
#endif
#ifdef ENAMETOOLONG
          case ENAMETOOLONG:
            fprintf(stderr,"The directory name %s is too long.\n",
                    dirbuildup);
            break;
#endif
#ifdef EROFS
          case EROFS:
            fprintf(stderr,"%s resides on a read-only file system.\n",
                    dirbuildup);
            break;
#endif
#ifdef ENOSPC
          case ENOSPC:
            fprintf(stderr,"No space left on the file system that will "
                    "contain the directory %s.\n", dirbuildup);
            break;
#endif
#ifdef EDQUOT
          case EDQUOT:
            fprintf(stderr,"Cannot create directory %s because you "
                    "exceeded your quota.\n", dirbuildup);
            break;
#endif
          default :
            fprintf(stderr,"Error creating directory %s.\n", dirbuildup);
            break;
          }
          break; /* get out of loop */
        }
      }
    }
    tempdir = tempdir2;
  }
  free(dirbuildup);
  free(outdup);

  return result; /* 0 is fine, -1 is badness */
}