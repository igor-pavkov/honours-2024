string Tensor::SummarizeValue(int64_t max_entries, bool print_v2) const {
  const int64_t num_elts = NumElements();
  if (max_entries < 0) {
    max_entries = num_elts;
  }
  size_t limit = std::min(max_entries, num_elts);
  if ((limit > 0) && (buf_ == nullptr)) {
    return strings::StrCat("uninitialized Tensor of ", num_elts,
                           " elements of type ", dtype());
  }
  const char* data = limit > 0 ? tensor_data().data() : nullptr;
  switch (dtype()) {
    case DT_BFLOAT16:
      return SummarizeArray<bfloat16>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_HALF:
      return SummarizeArray<Eigen::half>(limit, num_elts, shape_, data,
                                         print_v2);
      break;
    case DT_FLOAT:
      return SummarizeArray<float>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_DOUBLE:
      return SummarizeArray<double>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_UINT32:
      return SummarizeArray<uint32>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_INT32:
      return SummarizeArray<int32>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_UINT8:
    case DT_QUINT8:
      return SummarizeArray<uint8>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_UINT16:
    case DT_QUINT16:
      return SummarizeArray<uint16>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_INT16:
    case DT_QINT16:
      return SummarizeArray<int16>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_INT8:
    case DT_QINT8:
      return SummarizeArray<int8>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_UINT64:
      return SummarizeArray<uint64>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_INT64:
      return SummarizeArray<int64_t>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_BOOL:
      // TODO(tucker): Is it better to emit "True False..."?  This
      // will emit "1 0..." which is more compact.
      return SummarizeArray<bool>(limit, num_elts, shape_, data, print_v2);
      break;
    case DT_STRING:
      return SummarizeArray<tstring>(limit, num_elts, shape_, data, print_v2);
      break;
    default: {
      // All irregular cases
      string ret;
      if (print_v2 && (dims() > 0)) {
        strings::StrAppend(&ret, "[");
      }
      // TODO(irving): Don't call flat every time around this
      // loop.
      for (size_t i = 0; i < limit; ++i) {
        if (i > 0) strings::StrAppend(&ret, " ");
        switch (dtype()) {
          case DT_VARIANT: {
            const Variant& v = flat<Variant>()(i);
            strings::StrAppend(&ret, "<", v.SummarizeValue(), ">");
          } break;
          case DT_RESOURCE: {
            const ResourceHandle& r = flat<ResourceHandle>()(i);
            strings::StrAppend(&ret, "<", r.SummarizeValue(), ">");
          } break;
          default:
            // TODO(zhifengc, josh11b): Pretty-print other types (bool,
            // complex64, quantized).
            strings::StrAppend(&ret, "?");
        }
      }
      if (max_entries < num_elts) strings::StrAppend(&ret, "...");
      if (print_v2 && (dims() > 0)) {
        strings::StrAppend(&ret, "]");
      }
      return ret;
    }
  }
}