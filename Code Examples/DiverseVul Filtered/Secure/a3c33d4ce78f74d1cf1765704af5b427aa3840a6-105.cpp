void Http2Session::AddStream(Http2Stream* stream) {
  CHECK_GE(++statistics_.stream_count, 0);
  streams_[stream->id()] = BaseObjectPtr<Http2Stream>(stream);
  size_t size = streams_.size();
  if (size > statistics_.max_concurrent_streams)
    statistics_.max_concurrent_streams = size;
  IncrementCurrentSessionMemory(sizeof(*stream));
}