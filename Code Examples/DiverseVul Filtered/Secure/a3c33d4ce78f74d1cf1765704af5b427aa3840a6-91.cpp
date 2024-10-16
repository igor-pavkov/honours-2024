MaybeLocal<Object> Http2StreamPerformanceEntryTraits::GetDetails(
    Environment* env,
    const Http2StreamPerformanceEntry& entry) {
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

  SET(bytes_read_string, received_bytes)
  SET(bytes_written_string, sent_bytes)
  SET(id_string, id)
#undef SET

#define SET(name, val)                                                         \
  if (!obj->Set(                                                               \
          env->context(),                                                      \
          env->name(),                                                         \
          Number::New(                                                         \
              env->isolate(),                                                  \
              (entry.details.val - entry.details.start_time) / 1e6))           \
                  .IsJust()) {                                                 \
    return MaybeLocal<Object>();                                               \
  }

  SET(time_to_first_byte_string, first_byte)
  SET(time_to_first_byte_sent_string, first_byte_sent)
  SET(time_to_first_header_string, first_header)
#undef SET

  return obj;
}