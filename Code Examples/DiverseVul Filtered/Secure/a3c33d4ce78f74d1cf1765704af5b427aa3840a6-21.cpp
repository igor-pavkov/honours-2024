void Http2Session::Origin(const Origins& origins) {
  Http2Scope h2scope(this);
  CHECK_EQ(nghttp2_submit_origin(
      session_.get(),
      NGHTTP2_FLAG_NONE,
      *origins,
      origins.length()), 0);
}