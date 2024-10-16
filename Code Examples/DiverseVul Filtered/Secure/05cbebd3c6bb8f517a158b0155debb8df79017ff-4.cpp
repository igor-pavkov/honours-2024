ImmutableExecutorState::FrameInfo* ImmutableExecutorState::EnsureFrameInfo(
    const string& fname) {
  auto iter = frame_info_.find(fname);
  if (iter != frame_info_.end()) {
    return iter->second.get();
  } else {
    auto frame_info = absl::make_unique<FrameInfo>(fname);
    absl::string_view fname_view = frame_info->name;
    auto emplace_result =
        frame_info_.emplace(fname_view, std::move(frame_info));
    return emplace_result.first->second.get();
  }
}