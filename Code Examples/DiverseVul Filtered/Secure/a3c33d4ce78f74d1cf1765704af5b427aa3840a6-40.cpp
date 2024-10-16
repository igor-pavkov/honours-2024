void Http2Stream::OnTrailers() {
  Debug(this, "let javascript know we are ready for trailers");
  CHECK(!this->is_destroyed());
  Isolate* isolate = env()->isolate();
  HandleScope scope(isolate);
  Local<Context> context = env()->context();
  Context::Scope context_scope(context);
  set_has_trailers(false);
  MakeCallback(env()->http2session_on_stream_trailers_function(), 0, nullptr);
}