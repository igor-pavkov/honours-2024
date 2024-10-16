string RtmpProtocol::get_C1_key(const uint8_t *ptr){
    /* 764bytes key结构
    random-data: (offset)bytes
    key-data: 128bytes
    random-data: (764-offset-128-4)bytes
    offset: 4bytes
     */
    int offset = 0;
    for (int i = C1_SCHEMA_SIZE - C1_OFFSET_SIZE; i < C1_SCHEMA_SIZE; ++i) {
        offset += ptr[i];
    }
    offset %= (C1_SCHEMA_SIZE - C1_KEY_SIZE - C1_OFFSET_SIZE);
    string key((char *) ptr + offset, C1_KEY_SIZE);
    //DebugL << "key offset:" << offset << ",key:" << hexdump(key.data(),key.size());
    return key;
}