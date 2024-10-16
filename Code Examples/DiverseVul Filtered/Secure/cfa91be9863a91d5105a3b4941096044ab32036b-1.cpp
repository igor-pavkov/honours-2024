  void operator()(OpKernelContext* context, const T1* input_data,
                  int input_batches, int input_height, int input_width,
                  int input_depth, int input_offset, const T2* filter_data,
                  int filter_height, int filter_width, int filter_count,
                  int filter_offset, int stride, Padding padding,
                  T3* output_data, int output_height, int output_width,
                  int output_shift, int output_offset, int output_mult) {
    // Set up some constants we need for the output down-shifting and
    // saturation.
    const int32 highest = static_cast<int32>(Eigen::NumTraits<T3>::highest());
    const int32 lowest = static_cast<int32>(Eigen::NumTraits<T3>::lowest());

    // When we're converting the 32 bit accumulator to a lower bit depth, we
    // need to add on 0.5 in fixed-point terms to make the operation round half
    // up towards positive infinity, rather than a floor.
    // We also need to watch out for the case when there's no down shift,
    // because a left shift by a negative number gives undefined results.
    const int32 rounding = (output_shift < 1) ? 0 : (1 << (output_shift - 1));

    // The two different padding modes we support can be a bit confusing. SAME
    // means we're trying to produce an output image that's the same size as the
    // input. It's complicated by stride, which shrinks the output image by a
    // a factor, but it means we end up sampling from outside the borders of the
    // input. These out-of-bounds values are read as zeroes. VALID means only
    // produce output values where the filters can read all their values from
    // within the input image. It effectively removes the margins of the output
    // image compared to the one produced by SAME. Stride complicates this
    // definition though, because it can result in the right and bottom filter
    // patches sampling from outside the borders if it's greater than 1.
    // Most of the logic for sorting this all out is done before this function,
    // when we calculate the output size, but the positioning of the origin of
    // the filters is different between the two modes, since SAME positions the
    // first filter off the edge of the input.
    int filter_left_offset;
    int filter_top_offset;
    if (padding == VALID) {
      filter_left_offset =
          ((output_width - 1) * stride + filter_width - input_width + 1) / 2;
      filter_top_offset =
          ((output_height - 1) * stride + filter_height - input_height + 1) / 2;
    } else {
      filter_left_offset =
          ((output_width - 1) * stride + filter_width - input_width) / 2;
      filter_top_offset =
          ((output_height - 1) * stride + filter_height - input_height) / 2;
    }

    // If we've got multiple images in our input, work through each of them.
    for (int batch = 0; batch < input_batches; ++batch) {
      // Walk through all the output image values, sliding the filter to
      // different
      // positions in the input.
      for (int out_y = 0; out_y < output_height; ++out_y) {
        for (int out_x = 0; out_x < output_width; ++out_x) {
          // Each filter kernel produces one output channel.
          for (int out_channel = 0; out_channel < filter_count; ++out_channel) {
            // We're going to calculate a single output value, which means we
            // need to multiply a three dimensional kernel of weights against
            // the current location within the input image.
            /*
              *-------------------------------...
              |\ ^
              | \in_depth
              |  \ v
              |   *-------------------------------...
              |   |            ^
              |   |       in_y_origin
              |   |            v   \
              |   |<in_x_origin>*---*^
              |   |            \|   |filter_height
              .   |             *---*v
              .   |             <--->
                  .         filter_width
                  .
            */
            const int in_x_origin = (out_x * stride) - filter_left_offset;
            const int in_y_origin = (out_y * stride) - filter_top_offset;
            int32 total = 0;
            for (int filter_y = 0; filter_y < filter_height; ++filter_y) {
              for (int filter_x = 0; filter_x < filter_width; ++filter_x) {
                for (int in_channel = 0; in_channel < input_depth;
                     ++in_channel) {
                  const int in_x = in_x_origin + filter_x;
                  const int in_y = in_y_origin + filter_y;
                  int32 input_value;
                  // If the location is outside the bounds of the input image,
                  // use zero as a default value.
                  if ((in_x >= 0) && (in_x < input_width) && (in_y >= 0) &&
                      (in_y < input_height)) {
                    const T1 input_source_value =
                        input_data[(batch * input_height * input_width *
                                    input_depth) +
                                   (in_y * input_width * input_depth) +
                                   (in_x * input_depth) + in_channel];
                    // We're promoting the T1 type to a higher bit depth here as
                    // we do the subtraction.
                    input_value =
                        static_cast<int32>(input_source_value) - input_offset;
                  } else {
                    input_value = 0;
                  }
                  const T2 filter_source_value =
                      filter_data[(filter_y * filter_width * input_depth *
                                   filter_count) +
                                  (filter_x * input_depth * filter_count) +
                                  (in_channel * filter_count) + out_channel];
                  // Another promotion to 32 bit, as above.
                  const int32 filter_value =
                      static_cast<int32>(filter_source_value) - filter_offset;
                  total += (input_value * filter_value);
                }
              }
            }
            // Here we're applying scale factors to compress the 32 bit
            // accumulated total to a potentially lower bit depth.
            const int32_t output =
                ((((total + output_offset) * output_mult) + rounding) >>
                 output_shift);
            // We need to saturate the results against the largest and smallest
            // values that can be represented in this type.
            const int32 top_clamped_output = std::min(output, highest);
            const int32 clamped_output = std::max(top_clamped_output, lowest);
            output_data[(batch * output_height * output_width * filter_count) +
                        (out_y * output_width * filter_count) +
                        (out_x * filter_count) + out_channel] = clamped_output;
          }
        }
      }
    }
  }