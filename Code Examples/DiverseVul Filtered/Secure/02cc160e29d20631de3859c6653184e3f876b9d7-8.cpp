    Status SaveInternal(SerializationContext* ctx,
                        IteratorStateWriter* writer) override {
      mutex_lock l(mu_);
      TF_RETURN_IF_ERROR(writer->WriteScalar(Iterator::full_name("i"), i_));
      TF_RETURN_IF_ERROR(
          writer->WriteScalar(Iterator::full_name("iter_loc"), iter_.loc()));
      TF_RETURN_IF_ERROR(writer->WriteScalar(
          Iterator::full_name("next_non_empty_i_"), next_non_empty_i_));
      if (i_ <= next_non_empty_i_) {
        TF_RETURN_IF_ERROR(writer->WriteTensor(
            Iterator::full_name("next_indices_"), next_indices_));
        TF_RETURN_IF_ERROR(writer->WriteTensor(
            Iterator::full_name("next_values_"), next_values_));
      }
      return Status::OK();
    }