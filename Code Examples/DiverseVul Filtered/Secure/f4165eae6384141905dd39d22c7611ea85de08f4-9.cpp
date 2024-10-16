static uchar* get_key_column(GRANT_COLUMN *buff, size_t *length,
			    my_bool not_used MY_ATTRIBUTE((unused)))
{
  *length=buff->key_length;
  return (uchar*) buff->column;
}