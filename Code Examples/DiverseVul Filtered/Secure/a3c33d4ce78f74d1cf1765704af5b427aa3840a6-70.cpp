BaseObjectPtr<Http2Stream> Http2Session::RemoveStream(int32_t id) {
  BaseObjectPtr<Http2Stream> stream;
  if (streams_.empty())
    return stream;
  stream = FindStream(id);
  if (stream) {
    streams_.erase(id);
    DecrementCurrentSessionMemory(sizeof(*stream));
  }
  return stream;
}