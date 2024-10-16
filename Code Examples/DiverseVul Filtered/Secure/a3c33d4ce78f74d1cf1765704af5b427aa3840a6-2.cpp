void Http2Settings::Done(bool ack) {
  uint64_t end = uv_hrtime();
  double duration = (end - startTime_) / 1e6;

  Local<Value> argv[] = {
    ack ? v8::True(env()->isolate()) : v8::False(env()->isolate()),
    Number::New(env()->isolate(), duration)
  };
  MakeCallback(callback(), arraysize(argv), argv);
}