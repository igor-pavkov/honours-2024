Http2Stream::Provider::Provider(Http2Stream* stream, int options) {
  CHECK(!stream->is_destroyed());
  provider_.source.ptr = stream;
  empty_ = options & STREAM_OPTION_EMPTY_PAYLOAD;
}