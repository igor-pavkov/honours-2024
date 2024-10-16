  explicit DecodePaddedRawOp(OpKernelConstruction* context)
      : OpKernel(context) {
    OP_REQUIRES_OK(context, context->GetAttr("out_type", &out_type_));

    const bool host_is_little_endian = port::kLittleEndian;
    bool data_is_little_endian;
    OP_REQUIRES_OK(context,
                   context->GetAttr("little_endian", &data_is_little_endian));
    convert_data_endianness_ = host_is_little_endian != data_is_little_endian;
  }