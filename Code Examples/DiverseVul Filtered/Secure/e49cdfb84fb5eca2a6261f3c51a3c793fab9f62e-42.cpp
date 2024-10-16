void ok_inflater_free(ok_inflater *inflater) {
    if (inflater) {
        ok_png_allocator allocator = inflater->allocator;
        void *allocator_user_data = inflater->allocator_user_data;
        allocator.free(allocator_user_data, inflater->buffer);
        allocator.free(allocator_user_data, inflater->code_length_huffman);
        allocator.free(allocator_user_data, inflater->literal_huffman);
        allocator.free(allocator_user_data, inflater->distance_huffman);
        allocator.free(allocator_user_data, inflater->fixed_literal_huffman);
        allocator.free(allocator_user_data, inflater->fixed_distance_huffman);
        allocator.free(allocator_user_data, inflater);
    }
}