bool Http2Session::AddSettings(Local<Function> callback) {
  Local<Object> obj;
  if (!env()->http2settings_constructor_template()
          ->NewInstance(env()->context())
              .ToLocal(&obj)) {
    return false;
  }

  BaseObjectPtr<Http2Settings> settings =
      MakeDetachedBaseObject<Http2Settings>(this, obj, callback, 0);
  if (!settings)
    return false;

  if (outstanding_settings_.size() == max_outstanding_settings_) {
    settings->Done(false);
    return false;
  }

  IncrementCurrentSessionMemory(sizeof(*settings));
  settings->Send();
  outstanding_settings_.emplace(std::move(settings));
  return true;
}