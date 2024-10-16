AP_DECLARE(void) ap_finalize_request_protocol(request_rec *r)
{
    int status = ap_discard_request_body(r);

    /* tell the filter chain there is no more content coming */
    if (status) {
        error_output_stream(r, status);
    }
    if (!r->eos_sent) {
        end_output_stream(r);
    }
}