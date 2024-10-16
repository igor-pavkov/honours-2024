AP_DECLARE(void) ap_note_basic_auth_failure(request_rec *r)
{
    ap_note_auth_failure(r);
}