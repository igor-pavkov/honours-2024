apr_status_t modsecurity_request_body_retrieve_end(modsec_rec *msr) {
    if (msr->msc_reqbody_storage == MSC_REQBODY_DISK) {
        if (msr->msc_reqbody_fd > 0) {
            close(msr->msc_reqbody_fd);
            msr->msc_reqbody_fd = -1;
        }
    }

    return 1;
}