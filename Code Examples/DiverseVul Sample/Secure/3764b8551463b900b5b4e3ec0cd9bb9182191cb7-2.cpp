static int send_connect_method_response (struct conn_s *connptr)
{
        return write_message (connptr->client_fd,
                              "HTTP/1.%u 200 Connection established\r\n"
                              "Proxy-agent: " PACKAGE "/" VERSION "\r\n"
                              "\r\n", connptr->protocol.major != 1 ? 0 :
                                      connptr->protocol.minor);
}