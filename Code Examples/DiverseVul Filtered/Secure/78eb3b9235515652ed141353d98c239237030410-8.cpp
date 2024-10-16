static int req_ap_get_server_port(request_rec *r)
{
    return (int) ap_get_server_port(r);
}