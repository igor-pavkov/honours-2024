static int ftruncate64 (int fd, curl_off_t where)
{
  curl_off_t curr;
  int rc = 0;

  if ((curr = _lseeki64(fd, 0, SEEK_CUR)) < 0)
     return -1;

  if (_lseeki64(fd, where, SEEK_SET) < 0)
     return -1;

  if (write(fd, 0, 0) < 0)
     rc = -1;
  _lseeki64(fd, curr, SEEK_SET);
  return rc;
}