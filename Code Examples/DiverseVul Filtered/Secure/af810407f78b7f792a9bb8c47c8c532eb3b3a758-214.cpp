bool handler::ha_was_semi_consistent_read()
{
  bool result= was_semi_consistent_read();
  if (result)
    increment_statistics(&SSV::ha_read_retry_count);
  return result;
}