static intList *get_vlan_table(VirtIONet *n)
{
    intList *list;
    int i, j;

    list = NULL;
    for (i = 0; i < MAX_VLAN >> 5; i++) {
        for (j = 0; n->vlans[i] && j <= 0x1f; j++) {
            if (n->vlans[i] & (1U << j)) {
                QAPI_LIST_PREPEND(list, (i << 5) + j);
            }
        }
    }

    return list;
}