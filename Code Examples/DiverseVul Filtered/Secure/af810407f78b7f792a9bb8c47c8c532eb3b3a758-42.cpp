static my_bool file_ext_exists(char *path, size_t path_len, const char *ext)
{
  strmake(path + path_len, ext, FN_REFLEN - path_len);
  return !access(path, F_OK);
}