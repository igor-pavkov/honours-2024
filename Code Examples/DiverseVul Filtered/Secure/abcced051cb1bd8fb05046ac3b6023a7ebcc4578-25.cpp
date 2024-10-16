  static void Fill(const ResourceHandle* data, size_t n, TensorProto* proto) {
    auto* handles = proto->mutable_resource_handle_val();
    handles->Clear();
    for (size_t i = 0; i < n; i++) {
      data[i].AsProto(handles->Add());
    }
  }