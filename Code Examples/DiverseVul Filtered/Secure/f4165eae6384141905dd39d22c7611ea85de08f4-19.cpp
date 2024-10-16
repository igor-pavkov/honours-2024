static uchar* get_grant_table(GRANT_NAME *buff, size_t *length,
			     my_bool not_used MY_ATTRIBUTE((unused)))
{
  *length=buff->key_length;
  return (uchar*) buff->hash_key;
}