bool HasHttp2Observer(Environment* env) {
  AliasedUint32Array& observers = env->performance_state()->observers;
  return observers[performance::NODE_PERFORMANCE_ENTRY_TYPE_HTTP2] != 0;
}