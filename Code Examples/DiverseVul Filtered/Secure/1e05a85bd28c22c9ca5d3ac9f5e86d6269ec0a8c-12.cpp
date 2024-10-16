Http::Stream::sendStartOfMessage(HttpReply *rep, StoreIOBuffer bodyData)
{
    prepareReply(rep);
    assert(rep);
    MemBuf *mb = rep->pack();

    // dump now, so we do not output any body.
    debugs(11, 2, "HTTP Client " << clientConnection);
    debugs(11, 2, "HTTP Client REPLY:\n---------\n" << mb->buf << "\n----------");

    /* Save length of headers for persistent conn checks */
    http->out.headers_sz = mb->contentSize();
#if HEADERS_LOG
    headersLog(0, 0, http->request->method, rep);
#endif

    if (bodyData.data && bodyData.length) {
        if (multipartRangeRequest())
            packRange(bodyData, mb);
        else if (http->request->flags.chunkedReply) {
            packChunk(bodyData, *mb);
        } else {
            size_t length = lengthToSend(bodyData.range());
            noteSentBodyBytes(length);
            mb->append(bodyData.data, length);
        }
    }

    getConn()->write(mb);
    delete mb;
}