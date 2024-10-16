int Http2Stream::ReadStop() {
  CHECK(!this->is_destroyed());
  if (!is_reading())
    return 0;
  set_paused();
  Debug(this, "reading stopped");
  return 0;
}