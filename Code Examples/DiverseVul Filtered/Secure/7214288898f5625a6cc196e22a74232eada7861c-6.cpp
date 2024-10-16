static void win_update_buffer_size(curl_socket_t sockfd)
{
  int result;
  ULONG ideal;
  DWORD ideallen;
  result = WSAIoctl(sockfd, SIO_IDEAL_SEND_BACKLOG_QUERY, 0, 0,
                    &ideal, sizeof(ideal), &ideallen, 0, 0);
  if(result == 0) {
    setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,
               (const char *)&ideal, sizeof(ideal));
  }
}