MaybeLocal<Object> Http2SessionPerformanceEntryTraits::GetDetails(
    Environment* env,
    const Http2SessionPerformanceEntry& entry) {
  Local<Object> obj = Object::New(env->isolate());

#define SET(name, val)                                                         \
  if (!obj->Set(                                                               \
          env->context(),                                                      \
          env->name(),                                                         \
          Number::New(                                                         \
            env->isolate(),                                                    \
            static_cast<double>(entry.details.val))).IsJust()) {               \
    return MaybeLocal<Object>();                                               \
  }

  SET(bytes_written_string, data_sent)
  SET(bytes_read_string, data_received)
  SET(frames_received_string, frame_count)
  SET(frames_sent_string, frame_sent)
  SET(max_concurrent_streams_string, max_concurrent_streams)
  SET(ping_rtt_string, ping_rtt)
  SET(stream_average_duration_string, stream_average_duration)
  SET(stream_count_string, stream_count)

  if (!obj->Set(
          env->context(),
          env->type_string(),
          OneByteString(
              env->isolate(),
              (entry.details.session_type == NGHTTP2_SESSION_SERVER)
                  ? "server" : "client")).IsJust()) {
    return MaybeLocal<Object>();
  }

#undef SET
  return obj;
}