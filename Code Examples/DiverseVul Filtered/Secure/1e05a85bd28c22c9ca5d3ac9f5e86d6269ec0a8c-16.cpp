Http::Stream::packRange(StoreIOBuffer const &source, MemBuf *mb)
{
    HttpHdrRangeIter * i = &http->range_iter;
    Range<int64_t> available(source.range());
    char const *buf = source.data;

    while (i->currentSpec() && available.size()) {
        const size_t copy_sz = lengthToSend(available);
        if (copy_sz) {
            // intersection of "have" and "need" ranges must not be empty
            assert(http->out.offset < i->currentSpec()->offset + i->currentSpec()->length);
            assert(http->out.offset + (int64_t)available.size() > i->currentSpec()->offset);

            /*
             * put boundary and headers at the beginning of a range in a
             * multi-range
             */
            if (http->multipartRangeRequest() && i->debt() == i->currentSpec()->length) {
                assert(http->memObject());
                clientPackRangeHdr(
                    http->memObject()->getReply(),  /* original reply */
                    i->currentSpec(),       /* current range */
                    i->boundary,    /* boundary, the same for all */
                    mb);
            }

            // append content
            debugs(33, 3, "appending " << copy_sz << " bytes");
            noteSentBodyBytes(copy_sz);
            mb->append(buf, copy_sz);

            // update offsets
            available.start += copy_sz;
            buf += copy_sz;
        }

        if (!canPackMoreRanges()) {
            debugs(33, 3, "Returning because !canPackMoreRanges.");
            if (i->debt() == 0)
                // put terminating boundary for multiparts
                clientPackTermBound(i->boundary, mb);
            return;
        }

        int64_t nextOffset = getNextRangeOffset();
        assert(nextOffset >= http->out.offset);
        int64_t skip = nextOffset - http->out.offset;
        /* adjust for not to be transmitted bytes */
        http->out.offset = nextOffset;

        if (available.size() <= (uint64_t)skip)
            return;

        available.start += skip;
        buf += skip;

        if (copy_sz == 0)
            return;
    }
}