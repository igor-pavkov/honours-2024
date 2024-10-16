void Http2Session::EmitStatistics() {
  if (LIKELY(!HasHttp2Observer(env())))
    return;

  double start = statistics_.start_time / 1e6;
  double duration = (PERFORMANCE_NOW() / 1e6) - start;

  std::unique_ptr<Http2SessionPerformanceEntry> entry =
      std::make_unique<Http2SessionPerformanceEntry>(
          "Http2Session",
          start,
          duration,
          statistics_);

  env()->SetImmediate([entry = std::move(entry)](Environment* env) {
    if (HasHttp2Observer(env))
      entry->Notify(env);
  });
}