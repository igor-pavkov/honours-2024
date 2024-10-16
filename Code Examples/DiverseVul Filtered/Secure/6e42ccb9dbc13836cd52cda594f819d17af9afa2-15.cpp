static int assign_pair(struct elem_to_channel e2c_vec[MAX_ELEM_ID],
                       uint8_t (*layout_map)[3], int offset, uint64_t left,
                       uint64_t right, int pos)
{
    if (layout_map[offset][0] == TYPE_CPE) {
        e2c_vec[offset] = (struct elem_to_channel) {
            .av_position  = left | right,
            .syn_ele      = TYPE_CPE,
            .elem_id      = layout_map[offset][1],
            .aac_position = pos
        };
        return 1;
    } else {
        e2c_vec[offset] = (struct elem_to_channel) {
            .av_position  = left,
            .syn_ele      = TYPE_SCE,
            .elem_id      = layout_map[offset][1],
            .aac_position = pos
        };
        e2c_vec[offset + 1] = (struct elem_to_channel) {
            .av_position  = right,
            .syn_ele      = TYPE_SCE,
            .elem_id      = layout_map[offset + 1][1],
            .aac_position = pos
        };
        return 2;
    }
}