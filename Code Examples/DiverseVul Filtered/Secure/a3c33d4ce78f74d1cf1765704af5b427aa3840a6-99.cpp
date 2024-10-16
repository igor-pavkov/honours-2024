void Http2Session::HandleHeadersFrame(const nghttp2_frame* frame) {
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);

  int32_t id = GetFrameID(frame);
  Debug(this, "handle headers frame for stream %d", id);
  BaseObjectPtr<Http2Stream> stream = FindStream(id);

  // If the stream has already been destroyed, ignore.
  if (!stream || stream->is_destroyed())
    return;

  // The headers are stored as a vector of Http2Header instances.
  // The following converts that into a JS array with the structure:
  // [name1, value1, name2, value2, name3, value3, name3, value4] and so on.
  // That array is passed up to the JS layer and converted into an Object form
  // like {name1: value1, name2: value2, name3: [value3, value4]}. We do it
  // this way for performance reasons (it's faster to generate and pass an
  // array than it is to generate and pass the object).

  MaybeStackBuffer<Local<Value>, 64> headers_v(stream->headers_count() * 2);
  MaybeStackBuffer<Local<Value>, 32> sensitive_v(stream->headers_count());
  size_t sensitive_count = 0;

  stream->TransferHeaders([&](const Http2Header& header, size_t i) {
    headers_v[i * 2] = header.GetName(this).ToLocalChecked();
    headers_v[i * 2 + 1] = header.GetValue(this).ToLocalChecked();
    if (header.flags() & NGHTTP2_NV_FLAG_NO_INDEX)
      sensitive_v[sensitive_count++] = headers_v[i * 2];
  });
  CHECK_EQ(stream->headers_count(), 0);

  DecrementCurrentSessionMemory(stream->current_headers_length_);
  stream->current_headers_length_ = 0;

  Local<Value> args[] = {
    stream->object(),
    Integer::New(isolate, id),
    Integer::New(isolate, stream->headers_category()),
    Integer::New(isolate, frame->hd.flags),
    Array::New(isolate, headers_v.out(), headers_v.length()),
    Array::New(isolate, sensitive_v.out(), sensitive_count),
  };
  MakeCallback(env()->http2session_on_headers_function(),
               arraysize(args), args);
}