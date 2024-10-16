static intList *get_vlan_table(VirtIONet *n)
{
    intList *list, *entry;
    int i, j;

    list = NULL;
    for (i = 0; i < MAX_VLAN >> 5; i++) {
        for (j = 0; n->vlans[i] && j <= 0x1f; j++) {
            if (n->vlans[i] & (1U << j)) {
                entry = g_malloc0(sizeof(*entry));
                entry->value = (i << 5) + j;
                entry->next = list;
                list = entry;
            }
        }
    }

    return list;
}