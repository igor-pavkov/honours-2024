int handler::index_read_idx_map(uchar * buf, uint index, const uchar * key,
                                key_part_map keypart_map,
                                enum ha_rkey_function find_flag)
{
  int error, UNINIT_VAR(error1);

  error= ha_index_init(index, 0);
  if (likely(!error))
  {
    error= index_read_map(buf, key, keypart_map, find_flag);
    error1= ha_index_end();
  }
  return error ? error : error1;
}