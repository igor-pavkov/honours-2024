bool Http2Session::AddPing(const uint8_t* payload, Local<Function> callback) {
  Local<Object> obj;
  if (!env()->http2ping_constructor_template()
          ->NewInstance(env()->context())
              .ToLocal(&obj)) {
    return false;
  }

  BaseObjectPtr<Http2Ping> ping =
      MakeDetachedBaseObject<Http2Ping>(this, obj, callback);
  if (!ping)
    return false;

  if (outstanding_pings_.size() == max_outstanding_pings_) {
    ping->Done(false);
    return false;
  }

  IncrementCurrentSessionMemory(sizeof(*ping));
  // The Ping itself is an Async resource. When the acknowledgement is received,
  // the callback will be invoked and a notification sent out to JS land. The
  // notification will include the duration of the ping, allowing the round
  // trip to be measured.
  ping->Send(payload);

  outstanding_pings_.emplace(std::move(ping));
  return true;
}