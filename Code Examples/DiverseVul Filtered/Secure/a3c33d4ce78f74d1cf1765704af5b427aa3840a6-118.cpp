Origins::Origins(
    Environment* env,
    Local<String> origin_string,
    size_t origin_count)
    : count_(origin_count) {
  int origin_string_len = origin_string->Length();
  if (count_ == 0) {
    CHECK_EQ(origin_string_len, 0);
    return;
  }

  buf_ = AllocatedBuffer::AllocateManaged(
      env,
      (alignof(nghttp2_origin_entry) - 1) +
       count_ * sizeof(nghttp2_origin_entry) +
                              origin_string_len);

  // Make sure the start address is aligned appropriately for an nghttp2_nv*.
  char* start = AlignUp(buf_.data(), alignof(nghttp2_origin_entry));
  char* origin_contents = start + (count_ * sizeof(nghttp2_origin_entry));
  nghttp2_origin_entry* const nva =
      reinterpret_cast<nghttp2_origin_entry*>(start);

  CHECK_LE(origin_contents + origin_string_len, buf_.data() + buf_.size());
  CHECK_EQ(origin_string->WriteOneByte(
               env->isolate(),
               reinterpret_cast<uint8_t*>(origin_contents),
               0,
               origin_string_len,
               String::NO_NULL_TERMINATION),
           origin_string_len);

  size_t n = 0;
  char* p;
  for (p = origin_contents; p < origin_contents + origin_string_len; n++) {
    if (n >= count_) {
      static uint8_t zero = '\0';
      nva[0].origin = &zero;
      nva[0].origin_len = 1;
      count_ = 1;
      return;
    }

    nva[n].origin = reinterpret_cast<uint8_t*>(p);
    nva[n].origin_len = strlen(p);
    p += nva[n].origin_len + 1;
  }
}