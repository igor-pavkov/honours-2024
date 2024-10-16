int send_probe_requests(struct wif *wi[], int cards)
{
    int i=0;
    for(i=0; i<cards; i++)
    {
        send_probe_request(wi[i]);
    }
    return 0;
}