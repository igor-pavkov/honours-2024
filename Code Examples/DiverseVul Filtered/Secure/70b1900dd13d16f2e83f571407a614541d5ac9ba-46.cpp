static int ftpfilemethod(struct Configurable *config, char *str)
{
  if(curlx_strequal("singlecwd", str))
    return CURLFTPMETHOD_SINGLECWD;
  if(curlx_strequal("nocwd", str))
    return CURLFTPMETHOD_NOCWD;
  if(curlx_strequal("multicwd", str))
    return CURLFTPMETHOD_MULTICWD;
  warnf(config, "unrecognized ftp file method '%s', using default\n", str);
  return CURLFTPMETHOD_MULTICWD;
}