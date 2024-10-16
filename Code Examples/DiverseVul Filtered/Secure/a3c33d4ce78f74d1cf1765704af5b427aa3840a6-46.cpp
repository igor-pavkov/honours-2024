BaseObjectPtr<Http2Ping> Http2Session::PopPing() {
  BaseObjectPtr<Http2Ping> ping;
  if (!outstanding_pings_.empty()) {
    ping = std::move(outstanding_pings_.front());
    outstanding_pings_.pop();
    DecrementCurrentSessionMemory(sizeof(*ping));
  }
  return ping;
}