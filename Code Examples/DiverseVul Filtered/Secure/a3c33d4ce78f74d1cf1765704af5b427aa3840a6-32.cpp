BaseObjectPtr<Http2Settings> Http2Session::PopSettings() {
  BaseObjectPtr<Http2Settings> settings;
  if (!outstanding_settings_.empty()) {
    settings = std::move(outstanding_settings_.front());
    outstanding_settings_.pop();
    DecrementCurrentSessionMemory(sizeof(*settings));
  }
  return settings;
}