void Http2Stream::DecrementAvailableOutboundLength(size_t amount) {
  available_outbound_length_ -= amount;
  session_->DecrementCurrentSessionMemory(amount);
}