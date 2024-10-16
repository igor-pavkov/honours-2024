port resolvePort(node_t * n, node_t * other, port * oldport)
{
    port rv;
    char *compass = closestSide(n, other, oldport);

    /* transfer name pointer; all other necessary fields will be regenerated */
    rv.name = oldport->name;
    compassPort(n, oldport->bp, &rv, compass, oldport->side, NULL);

    return rv;
}