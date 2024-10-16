void RGWListBuckets_ObjStore_S3::send_response_begin(bool has_buckets)
{
  if (op_ret)
    set_req_state_err(s, op_ret);
  dump_errno(s);
  dump_start(s);
  // Explicitly use chunked transfer encoding so that we can stream the result
  // to the user without having to wait for the full length of it.
  end_header(s, NULL, "application/xml", CHUNKED_TRANSFER_ENCODING);

  if (! op_ret) {
    list_all_buckets_start(s);
    dump_owner(s, s->user->get_id(), s->user->get_display_name());
    s->formatter->open_array_section("Buckets");
    sent_data = true;
  }
}