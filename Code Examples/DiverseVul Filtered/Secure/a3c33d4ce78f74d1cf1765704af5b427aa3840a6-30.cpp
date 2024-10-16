void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context,
                void* priv) {
  Environment* env = Environment::GetCurrent(context);
  Isolate* isolate = env->isolate();
  HandleScope handle_scope(isolate);

  Http2State* const state = env->AddBindingData<Http2State>(context, target);
  if (state == nullptr) return;

#define SET_STATE_TYPEDARRAY(name, field)             \
  target->Set(context,                                \
              FIXED_ONE_BYTE_STRING(isolate, (name)), \
              (field)).FromJust()

  // Initialize the buffer used to store the session state
  SET_STATE_TYPEDARRAY(
    "sessionState", state->session_state_buffer.GetJSArray());
  // Initialize the buffer used to store the stream state
  SET_STATE_TYPEDARRAY(
    "streamState", state->stream_state_buffer.GetJSArray());
  SET_STATE_TYPEDARRAY(
    "settingsBuffer", state->settings_buffer.GetJSArray());
  SET_STATE_TYPEDARRAY(
    "optionsBuffer", state->options_buffer.GetJSArray());
  SET_STATE_TYPEDARRAY(
    "streamStats", state->stream_stats_buffer.GetJSArray());
  SET_STATE_TYPEDARRAY(
    "sessionStats", state->session_stats_buffer.GetJSArray());
#undef SET_STATE_TYPEDARRAY

  NODE_DEFINE_CONSTANT(target, kBitfield);
  NODE_DEFINE_CONSTANT(target, kSessionPriorityListenerCount);
  NODE_DEFINE_CONSTANT(target, kSessionFrameErrorListenerCount);
  NODE_DEFINE_CONSTANT(target, kSessionMaxInvalidFrames);
  NODE_DEFINE_CONSTANT(target, kSessionMaxRejectedStreams);
  NODE_DEFINE_CONSTANT(target, kSessionUint8FieldCount);

  NODE_DEFINE_CONSTANT(target, kSessionHasRemoteSettingsListeners);
  NODE_DEFINE_CONSTANT(target, kSessionRemoteSettingsIsUpToDate);
  NODE_DEFINE_CONSTANT(target, kSessionHasPingListeners);
  NODE_DEFINE_CONSTANT(target, kSessionHasAltsvcListeners);

  // Method to fetch the nghttp2 string description of an nghttp2 error code
  env->SetMethod(target, "nghttp2ErrorString", HttpErrorString);
  env->SetMethod(target, "refreshDefaultSettings", RefreshDefaultSettings);
  env->SetMethod(target, "packSettings", PackSettings);
  env->SetMethod(target, "setCallbackFunctions", SetCallbackFunctions);

  Local<FunctionTemplate> ping = FunctionTemplate::New(env->isolate());
  ping->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "Http2Ping"));
  ping->Inherit(AsyncWrap::GetConstructorTemplate(env));
  Local<ObjectTemplate> pingt = ping->InstanceTemplate();
  pingt->SetInternalFieldCount(Http2Ping::kInternalFieldCount);
  env->set_http2ping_constructor_template(pingt);

  Local<FunctionTemplate> setting = FunctionTemplate::New(env->isolate());
  setting->Inherit(AsyncWrap::GetConstructorTemplate(env));
  Local<ObjectTemplate> settingt = setting->InstanceTemplate();
  settingt->SetInternalFieldCount(AsyncWrap::kInternalFieldCount);
  env->set_http2settings_constructor_template(settingt);

  Local<FunctionTemplate> stream = FunctionTemplate::New(env->isolate());
  env->SetProtoMethod(stream, "id", Http2Stream::GetID);
  env->SetProtoMethod(stream, "destroy", Http2Stream::Destroy);
  env->SetProtoMethod(stream, "priority", Http2Stream::Priority);
  env->SetProtoMethod(stream, "pushPromise", Http2Stream::PushPromise);
  env->SetProtoMethod(stream, "info", Http2Stream::Info);
  env->SetProtoMethod(stream, "trailers", Http2Stream::Trailers);
  env->SetProtoMethod(stream, "respond", Http2Stream::Respond);
  env->SetProtoMethod(stream, "rstStream", Http2Stream::RstStream);
  env->SetProtoMethod(stream, "refreshState", Http2Stream::RefreshState);
  stream->Inherit(AsyncWrap::GetConstructorTemplate(env));
  StreamBase::AddMethods(env, stream);
  Local<ObjectTemplate> streamt = stream->InstanceTemplate();
  streamt->SetInternalFieldCount(StreamBase::kInternalFieldCount);
  env->set_http2stream_constructor_template(streamt);
  env->SetConstructorFunction(target, "Http2Stream", stream);

  Local<FunctionTemplate> session =
      env->NewFunctionTemplate(Http2Session::New);
  session->InstanceTemplate()->SetInternalFieldCount(
      Http2Session::kInternalFieldCount);
  session->Inherit(AsyncWrap::GetConstructorTemplate(env));
  env->SetProtoMethod(session, "origin", Http2Session::Origin);
  env->SetProtoMethod(session, "altsvc", Http2Session::AltSvc);
  env->SetProtoMethod(session, "ping", Http2Session::Ping);
  env->SetProtoMethod(session, "consume", Http2Session::Consume);
  env->SetProtoMethod(session, "receive", Http2Session::Receive);
  env->SetProtoMethod(session, "destroy", Http2Session::Destroy);
  env->SetProtoMethod(session, "goaway", Http2Session::Goaway);
  env->SetProtoMethod(session, "settings", Http2Session::Settings);
  env->SetProtoMethod(session, "request", Http2Session::Request);
  env->SetProtoMethod(session, "setNextStreamID",
                      Http2Session::SetNextStreamID);
  env->SetProtoMethod(session, "setLocalWindowSize",
                      Http2Session::SetLocalWindowSize);
  env->SetProtoMethod(session, "updateChunksSent",
                      Http2Session::UpdateChunksSent);
  env->SetProtoMethod(session, "refreshState", Http2Session::RefreshState);
  env->SetProtoMethod(
      session, "localSettings",
      Http2Session::RefreshSettings<nghttp2_session_get_local_settings>);
  env->SetProtoMethod(
      session, "remoteSettings",
      Http2Session::RefreshSettings<nghttp2_session_get_remote_settings>);
  env->SetConstructorFunction(target, "Http2Session", session);

  Local<Object> constants = Object::New(isolate);

  // This does allocate one more slot than needed but it's not used.
#define V(name) FIXED_ONE_BYTE_STRING(isolate, #name),
  Local<Value> error_code_names[] = {
    HTTP2_ERROR_CODES(V)
  };
#undef V

  Local<Array> name_for_error_code =
      Array::New(
          isolate,
          error_code_names,
          arraysize(error_code_names));

  target->Set(context,
              FIXED_ONE_BYTE_STRING(isolate, "nameForErrorCode"),
              name_for_error_code).Check();

#define V(constant) NODE_DEFINE_HIDDEN_CONSTANT(constants, constant);
  HTTP2_HIDDEN_CONSTANTS(V)
#undef V

#define V(constant) NODE_DEFINE_CONSTANT(constants, constant);
  HTTP2_CONSTANTS(V)
#undef V

  // NGHTTP2_DEFAULT_WEIGHT is a macro and not a regular define
  // it won't be set properly on the constants object if included
  // in the HTTP2_CONSTANTS macro.
  NODE_DEFINE_CONSTANT(constants, NGHTTP2_DEFAULT_WEIGHT);

#define V(NAME, VALUE)                                          \
  NODE_DEFINE_STRING_CONSTANT(constants, "HTTP2_HEADER_" # NAME, VALUE);
  HTTP_KNOWN_HEADERS(V)
#undef V

#define V(NAME, VALUE)                                          \
  NODE_DEFINE_STRING_CONSTANT(constants, "HTTP2_METHOD_" # NAME, VALUE);
  HTTP_KNOWN_METHODS(V)
#undef V

#define V(name, _) NODE_DEFINE_CONSTANT(constants, HTTP_STATUS_##name);
  HTTP_STATUS_CODES(V)
#undef V

  target->Set(context, env->constants_string(), constants).Check();
}