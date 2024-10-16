void Http2Session::Consume(Local<Object> stream_obj) {
  StreamBase* stream = StreamBase::FromObject(stream_obj);
  stream->PushStreamListener(this);
  Debug(this, "i/o stream consumed");
}