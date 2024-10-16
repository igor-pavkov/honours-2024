string RtmpProtocol::get_C1_digest(const uint8_t *ptr,char **digestPos){
    /* 764bytes digest结构
    offset: 4bytes
    random-data: (offset)bytes
    digest-data: 32bytes
    random-data: (764-4-offset-32)bytes
     */
    int offset = 0;
    for (int i = 0; i < C1_OFFSET_SIZE; ++i) {
        offset += ptr[i];
    }
    offset %= (C1_SCHEMA_SIZE - C1_DIGEST_SIZE - C1_OFFSET_SIZE);
    *digestPos = (char *) ptr + C1_OFFSET_SIZE + offset;
    string digest(*digestPos, C1_DIGEST_SIZE);
    //DebugL << "digest offset:" << offset << ",digest:" << hexdump(digest.data(),digest.size());
    return digest;
}