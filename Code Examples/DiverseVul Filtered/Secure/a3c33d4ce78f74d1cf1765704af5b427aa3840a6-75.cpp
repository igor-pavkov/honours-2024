void HttpErrorString(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  uint32_t val = args[0]->Uint32Value(env->context()).ToChecked();
  args.GetReturnValue().Set(
      OneByteString(
          env->isolate(),
          reinterpret_cast<const uint8_t*>(nghttp2_strerror(val))));
}