void RefreshDefaultSettings(const FunctionCallbackInfo<Value>& args) {
  Http2State* state = Environment::GetBindingData<Http2State>(args);
  Http2Settings::RefreshDefaults(state);
}