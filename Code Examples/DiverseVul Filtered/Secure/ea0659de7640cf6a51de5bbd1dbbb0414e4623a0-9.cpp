TmEcode StreamTcpThreadInit(ThreadVars *tv, void *initdata, void **data)
{
    SCEnter();
    StreamTcpThread *stt = SCMalloc(sizeof(StreamTcpThread));
    if (unlikely(stt == NULL))
        SCReturnInt(TM_ECODE_FAILED);
    memset(stt, 0, sizeof(StreamTcpThread));
    stt->ssn_pool_id = -1;

    *data = (void *)stt;

    stt->counter_tcp_sessions = StatsRegisterCounter("tcp.sessions", tv);
    stt->counter_tcp_ssn_memcap = StatsRegisterCounter("tcp.ssn_memcap_drop", tv);
    stt->counter_tcp_pseudo = StatsRegisterCounter("tcp.pseudo", tv);
    stt->counter_tcp_pseudo_failed = StatsRegisterCounter("tcp.pseudo_failed", tv);
    stt->counter_tcp_invalid_checksum = StatsRegisterCounter("tcp.invalid_checksum", tv);
    stt->counter_tcp_no_flow = StatsRegisterCounter("tcp.no_flow", tv);
    stt->counter_tcp_syn = StatsRegisterCounter("tcp.syn", tv);
    stt->counter_tcp_synack = StatsRegisterCounter("tcp.synack", tv);
    stt->counter_tcp_rst = StatsRegisterCounter("tcp.rst", tv);
    stt->counter_tcp_midstream_pickups = StatsRegisterCounter("tcp.midstream_pickups", tv);
    stt->counter_tcp_wrong_thread = StatsRegisterCounter("tcp.pkt_on_wrong_thread", tv);

    /* init reassembly ctx */
    stt->ra_ctx = StreamTcpReassembleInitThreadCtx(tv);
    if (stt->ra_ctx == NULL)
        SCReturnInt(TM_ECODE_FAILED);

    stt->ra_ctx->counter_tcp_segment_memcap = StatsRegisterCounter("tcp.segment_memcap_drop", tv);
    stt->ra_ctx->counter_tcp_stream_depth = StatsRegisterCounter("tcp.stream_depth_reached", tv);
    stt->ra_ctx->counter_tcp_reass_gap = StatsRegisterCounter("tcp.reassembly_gap", tv);
    stt->ra_ctx->counter_tcp_reass_overlap = StatsRegisterCounter("tcp.overlap", tv);
    stt->ra_ctx->counter_tcp_reass_overlap_diff_data = StatsRegisterCounter("tcp.overlap_diff_data", tv);

    stt->ra_ctx->counter_tcp_reass_data_normal_fail = StatsRegisterCounter("tcp.insert_data_normal_fail", tv);
    stt->ra_ctx->counter_tcp_reass_data_overlap_fail = StatsRegisterCounter("tcp.insert_data_overlap_fail", tv);
    stt->ra_ctx->counter_tcp_reass_list_fail = StatsRegisterCounter("tcp.insert_list_fail", tv);


    SCLogDebug("StreamTcp thread specific ctx online at %p, reassembly ctx %p",
                stt, stt->ra_ctx);

    SCMutexLock(&ssn_pool_mutex);
    if (ssn_pool == NULL) {
        ssn_pool = PoolThreadInit(1, /* thread */
                0, /* unlimited */
                stream_config.prealloc_sessions,
                sizeof(TcpSession),
                StreamTcpSessionPoolAlloc,
                StreamTcpSessionPoolInit, NULL,
                StreamTcpSessionPoolCleanup, NULL);
        stt->ssn_pool_id = 0;
        SCLogDebug("pool size %d, thread ssn_pool_id %d", PoolThreadSize(ssn_pool), stt->ssn_pool_id);
    } else {
        /* grow ssn_pool until we have a element for our thread id */
        stt->ssn_pool_id = PoolThreadGrow(ssn_pool,
                0, /* unlimited */
                stream_config.prealloc_sessions,
                sizeof(TcpSession),
                StreamTcpSessionPoolAlloc,
                StreamTcpSessionPoolInit, NULL,
                StreamTcpSessionPoolCleanup, NULL);
        SCLogDebug("pool size %d, thread ssn_pool_id %d", PoolThreadSize(ssn_pool), stt->ssn_pool_id);
    }
    SCMutexUnlock(&ssn_pool_mutex);
    if (stt->ssn_pool_id < 0 || ssn_pool == NULL) {
        SCLogError(SC_ERR_MEM_ALLOC, "failed to setup/expand stream session pool. Expand stream.memcap?");
        SCReturnInt(TM_ECODE_FAILED);
    }

    SCReturnInt(TM_ECODE_OK);
}