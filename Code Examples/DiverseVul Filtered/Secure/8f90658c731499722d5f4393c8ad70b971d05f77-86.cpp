int RGWPostObj_ObjStore_S3::complete_get_params()
{
  bool done;
  do {
    struct post_form_part part;
    int r = read_form_part_header(&part, done);
    if (r < 0) {
      return r;
    }

    ceph::bufferlist part_data;
    bool boundary;
    uint64_t chunk_size = s->cct->_conf->rgw_max_chunk_size;
    r = read_data(part.data, chunk_size, boundary, done);
    if (r < 0 || !boundary) {
      return -EINVAL;
    }

    /* Just reading the data but not storing any results of that. */
  } while (!done);

  return 0;
}