static int count_channels(uint8_t (*layout)[3], int tags)
{
    int i, sum = 0;
    for (i = 0; i < tags; i++) {
        int syn_ele = layout[i][0];
        int pos     = layout[i][2];
        sum += (1 + (syn_ele == TYPE_CPE)) *
               (pos != AAC_CHANNEL_OFF && pos != AAC_CHANNEL_CC);
    }
    return sum;
}