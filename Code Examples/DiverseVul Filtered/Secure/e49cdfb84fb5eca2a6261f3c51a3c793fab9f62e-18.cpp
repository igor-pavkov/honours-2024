ok_inflater *ok_inflater_init(bool nowrap, ok_png_allocator allocator, void *allocator_user_data) {
    ok_inflater *inflater = allocator.alloc(allocator_user_data, sizeof(ok_inflater));
    if (inflater) {
        memset(inflater, 0, sizeof(ok_inflater));
        inflater->nowrap = nowrap;
        inflater->allocator = allocator;
        inflater->allocator_user_data = allocator_user_data;
        inflater->state = (nowrap ? OK_INFLATER_STATE_READY_FOR_NEXT_BLOCK :
                           OK_INFLATER_STATE_READY_FOR_HEAD);
        inflater->buffer = ok_alloc(inflater, BUFFER_SIZE);
        inflater->code_length_huffman = ok_alloc(inflater, sizeof(ok_inflater_huffman_tree));
        inflater->literal_huffman = ok_alloc(inflater, sizeof(ok_inflater_huffman_tree));
        inflater->distance_huffman = ok_alloc(inflater, sizeof(ok_inflater_huffman_tree));

        if (!inflater->buffer ||
            !inflater->code_length_huffman ||
            !inflater->literal_huffman ||
            !inflater->distance_huffman) {
            ok_inflater_free(inflater);
            inflater = NULL;
        }
    }
    return inflater;
}