  static void Fill(const Variant* data, size_t n, TensorProto* proto) {
    auto* variant_values = proto->mutable_variant_val();
    variant_values->Clear();
    for (size_t i = 0; i < n; ++i) {
      VariantTensorData tmp;
      data[i].Encode(&tmp);
      tmp.ToProto(variant_values->Add());
    }
  }