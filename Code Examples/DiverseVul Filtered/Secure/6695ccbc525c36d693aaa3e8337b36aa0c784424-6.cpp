static void complete_nread(conn *c) {
    assert(c != NULL);
    assert(c->protocol == ascii_prot
           || c->protocol == binary_prot);

    if (c->protocol == ascii_prot) {
        complete_nread_ascii(c);
    } else if (c->protocol == binary_prot) {
        complete_nread_binary(c);
    }
}