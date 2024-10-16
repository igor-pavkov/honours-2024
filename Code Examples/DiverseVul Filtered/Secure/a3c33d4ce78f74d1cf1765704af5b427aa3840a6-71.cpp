void Http2Stream::Close(int32_t code) {
  CHECK(!this->is_destroyed());
  set_closed();
  code_ = code;
  Debug(this, "closed with code %d", code);
}