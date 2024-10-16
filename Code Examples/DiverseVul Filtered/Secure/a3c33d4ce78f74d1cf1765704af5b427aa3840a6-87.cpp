void Http2Stream::EmitStatistics() {
  CHECK_NOT_NULL(session());
  if (LIKELY(!HasHttp2Observer(env())))
    return;

  double start = statistics_.start_time / 1e6;
  double duration = (PERFORMANCE_NOW() / 1e6) - start;

  std::unique_ptr<Http2StreamPerformanceEntry> entry =
      std::make_unique<Http2StreamPerformanceEntry>(
          "Http2Stream",
          start,
          duration,
          statistics_);

  env()->SetImmediate([entry = move(entry)](Environment* env) {
    if (HasHttp2Observer(env))
      entry->Notify(env);
  });
}