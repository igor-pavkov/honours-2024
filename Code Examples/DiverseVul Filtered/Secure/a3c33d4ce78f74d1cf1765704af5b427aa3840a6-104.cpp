Http2Scope::~Http2Scope() {
  if (!session_) return;
  session_->set_in_scope(false);
  if (!session_->is_write_scheduled())
    session_->MaybeScheduleWrite();
}