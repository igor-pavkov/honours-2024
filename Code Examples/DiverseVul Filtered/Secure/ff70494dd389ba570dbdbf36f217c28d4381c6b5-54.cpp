int list_add_packet(struct pkt_buf **list, int length, unsigned char* packet)
{
    struct pkt_buf *next = *list;

    if(length <= 0) return 1;
    if(packet == NULL) return 1;
    if(list == NULL) return 1;

    *list = (struct pkt_buf*) malloc(sizeof(struct pkt_buf));
    if( *list == NULL ) return 1;
    (*list)->packet = (unsigned char*) malloc(length);
    if( (*list)->packet == NULL ) return 1;

    memcpy((*list)->packet,  packet, length);
    (*list)->next = next;
    (*list)->length = length;
    gettimeofday( &((*list)->ctime), NULL);

    return 0;
}