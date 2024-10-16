int RGWPostObj_ObjStore_S3::get_data(ceph::bufferlist& bl, bool& again)
{
  bool boundary;
  bool done;

  const uint64_t chunk_size = s->cct->_conf->rgw_max_chunk_size;
  int r = read_data(bl, chunk_size, boundary, done);
  if (r < 0) {
    return r;
  }

  if (boundary) {
    if (!done) {
      /* Reached end of data, let's drain the rest of the params */
      r = complete_get_params();
      if (r < 0) {
       return r;
      }
    }
  }

  again = !boundary;
  return bl.length();
}