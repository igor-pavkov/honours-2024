void Http2Stream::IncrementAvailableOutboundLength(size_t amount) {
  available_outbound_length_ += amount;
  session_->IncrementCurrentSessionMemory(amount);
}