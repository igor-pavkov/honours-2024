static int ftpcccmethod(struct Configurable *config, char *str)
{
  if(curlx_strequal("passive", str))
    return CURLFTPSSL_CCC_PASSIVE;
  if(curlx_strequal("active", str))
    return CURLFTPSSL_CCC_ACTIVE;
  warnf(config, "unrecognized ftp CCC method '%s', using default\n", str);
  return CURLFTPSSL_CCC_PASSIVE;
}