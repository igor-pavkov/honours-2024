BaseObjectPtr<Http2Stream> Http2Session::FindStream(int32_t id) {
  auto s = streams_.find(id);
  return s != streams_.end() ? s->second : BaseObjectPtr<Http2Stream>();
}