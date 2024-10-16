static void set_tac_srv_addr (unsigned int srv_no, const struct addrinfo *addr)
{
    _pam_log(LOG_DEBUG, "%s: server [%s]", __FUNCTION__,
                        tac_ntop(addr->ai_addr));

    if (srv_no < TAC_PLUS_MAXSERVERS) {
        if (addr) {
          if (addr->ai_family == AF_INET6) {
            tac_srv_addr[srv_no].ai_addr = (struct sockaddr *)&tac_sock6_addr[srv_no];
          } else {
            tac_srv_addr[srv_no].ai_addr = &tac_sock_addr[srv_no];
          }
          tac_copy_addr_info (&tac_srv_addr[srv_no], addr);
          tac_srv[srv_no].addr = &tac_srv_addr[srv_no];

          /*this code will copy the ipv6 address to a temp variable */
          /*and copies to global tac_srv array*/
          if (addr->ai_family == AF_INET6) {
            memset (&tac_sock6_addr[srv_no], 0, sizeof(struct sockaddr_in6));
            memcpy (&tac_sock6_addr[srv_no], (struct sockaddr_in6*)addr->ai_addr, sizeof(struct sockaddr_in6));
            tac_srv[srv_no].addr->ai_addr = (struct sockaddr *)&tac_sock6_addr[srv_no];
          }
          _pam_log(LOG_DEBUG, "%s: server %d after copy [%s]",  __FUNCTION__, srv_no,
                        tac_ntop(tac_srv[srv_no].addr->ai_addr));
        } 
        else {
            tac_srv[srv_no].addr = NULL;
        }
    }
}