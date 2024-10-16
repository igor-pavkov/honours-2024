void PackSettings(const FunctionCallbackInfo<Value>& args) {
  Http2State* state = Environment::GetBindingData<Http2State>(args);
  args.GetReturnValue().Set(Http2Settings::Pack(state));
}