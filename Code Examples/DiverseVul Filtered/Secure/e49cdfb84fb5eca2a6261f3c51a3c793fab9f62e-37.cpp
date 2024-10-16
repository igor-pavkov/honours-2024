static bool ok_inflater_dynamic_block_header(ok_inflater *inflater) {
    if (!ok_inflater_load_bits(inflater, 14)) {
        return false;
    } else {
        inflater->num_literal_codes = (int)ok_inflater_read_bits(inflater, 5) + 257;
        inflater->num_distance_codes = (int)ok_inflater_read_bits(inflater, 5) + 1;
        inflater->num_code_length_codes = (int)ok_inflater_read_bits(inflater, 4) + 4;

        for (int i = inflater->num_code_length_codes; i < OK_INFLATER_BIT_LENGTH_TABLE_LENGTH; i++) {
            inflater->tree_codes[OK_INFLATER_BIT_LENGTH_TABLE[i]] = 0;
        }

        inflater->state = OK_INFLATER_STATE_READING_DYNAMIC_CODE_LENGTHS;
        inflater->state_count = inflater->num_code_length_codes;
        return true;
    }
}