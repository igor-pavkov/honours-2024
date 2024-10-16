void SetCallbackFunctions(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  CHECK_EQ(args.Length(), 11);

#define SET_FUNCTION(arg, name)                                               \
  CHECK(args[arg]->IsFunction());                                             \
  env->set_http2session_on_ ## name ## _function(args[arg].As<Function>());

  SET_FUNCTION(0, error)
  SET_FUNCTION(1, priority)
  SET_FUNCTION(2, settings)
  SET_FUNCTION(3, ping)
  SET_FUNCTION(4, headers)
  SET_FUNCTION(5, frame_error)
  SET_FUNCTION(6, goaway_data)
  SET_FUNCTION(7, altsvc)
  SET_FUNCTION(8, origin)
  SET_FUNCTION(9, stream_trailers)
  SET_FUNCTION(10, stream_close)

#undef SET_FUNCTION
}