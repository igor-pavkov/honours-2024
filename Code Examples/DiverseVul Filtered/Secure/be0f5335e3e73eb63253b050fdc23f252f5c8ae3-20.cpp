AP_DECLARE(void) ap_finalize_sub_req_protocol(request_rec *sub)
{
    /* tell the filter chain there is no more content coming */
    if (!sub->eos_sent) {
        end_output_stream(sub);
    }
}