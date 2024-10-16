    Status RestoreInternal(IteratorContext* ctx,
                           IteratorStateReader* reader) override {
      mutex_lock l(mu_);
      TF_RETURN_IF_ERROR(reader->ReadScalar(Iterator::full_name("i"), &i_));
      int64_t iter_loc;
      TF_RETURN_IF_ERROR(
          reader->ReadScalar(Iterator::full_name("iter_loc"), &iter_loc));
      iter_ = group_iterable_.at(iter_loc);
      TF_RETURN_IF_ERROR(reader->ReadScalar(
          Iterator::full_name("next_non_empty_i_"), &next_non_empty_i_));
      if (i_ <= next_non_empty_i_) {
        TF_RETURN_IF_ERROR(reader->ReadTensor(
            Iterator::full_name("next_indices_"), &next_indices_));
        TF_RETURN_IF_ERROR(reader->ReadTensor(
            Iterator::full_name("next_values_"), &next_values_));
      }
      return Status::OK();
    }