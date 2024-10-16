void RtmpProtocol::sendRtmp(uint8_t type, uint32_t stream_index, const Buffer::Ptr &buf, uint32_t stamp, int chunk_id){
    if (chunk_id < 2 || chunk_id > 63) {
        auto strErr = StrPrinter << "不支持发送该类型的块流 ID:" << chunk_id << endl;
        throw std::runtime_error(strErr);
    }
    //是否有扩展时间戳
    bool ext_stamp = stamp >= 0xFFFFFF;

    //rtmp头
    BufferRaw::Ptr buffer_header = obtainBuffer();
    buffer_header->setCapacity(sizeof(RtmpHeader));
    buffer_header->setSize(sizeof(RtmpHeader));
    //对rtmp头赋值，如果使用整形赋值，在arm android上可能由于数据对齐导致总线错误的问题
    RtmpHeader *header = (RtmpHeader *) buffer_header->data();
    header->fmt = 0;
    header->chunk_id = chunk_id;
    header->type_id = type;
    set_be24(header->time_stamp, ext_stamp ? 0xFFFFFF : stamp);
    set_be24(header->body_size, (uint32_t)buf->size());
    set_le32(header->stream_index, stream_index);
    //发送rtmp头
    onSendRawData(std::move(buffer_header));

    //扩展时间戳字段
    BufferRaw::Ptr buffer_ext_stamp;
    if (ext_stamp) {
        //生成扩展时间戳
        buffer_ext_stamp = obtainBuffer();
        buffer_ext_stamp->setCapacity(4);
        buffer_ext_stamp->setSize(4);
        set_be32(buffer_ext_stamp->data(), stamp);
    }

    //生成一个字节的flag，标明是什么chunkId
    BufferRaw::Ptr buffer_flags = obtainBuffer();
    buffer_flags->setCapacity(1);
    buffer_flags->setSize(1);
    header = (RtmpHeader *) buffer_flags->data();
    header->fmt = 3;
    header->chunk_id = chunk_id;

    size_t offset = 0;
    size_t totalSize = sizeof(RtmpHeader);
    while (offset < buf->size()) {
        if (offset) {
            onSendRawData(buffer_flags);
            totalSize += 1;
        }
        if (ext_stamp) {
            //扩展时间戳
            onSendRawData(buffer_ext_stamp);
            totalSize += 4;
        }
        size_t chunk = min(_chunk_size_out, buf->size() - offset);
        onSendRawData(std::make_shared<BufferPartial>(buf, offset, chunk));
        totalSize += chunk;
        offset += chunk;
    }
    _bytes_sent += (uint32_t)totalSize;
    if (_windows_size > 0 && _bytes_sent - _bytes_sent_last >= _windows_size) {
        _bytes_sent_last = _bytes_sent;
        sendAcknowledgement(_bytes_sent);
    }
}