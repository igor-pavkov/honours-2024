static int r_flush(apr_vformatter_buff_t *buff)
{
    /* callback function passed to ap_vformatter to be called when
     * vformatter needs to write into buff and buff.curpos > buff.endpos */

    /* ap_vrprintf_data passed as a apr_vformatter_buff_t, which is then
     * "downcast" to an ap_vrprintf_data */
    struct ap_vrprintf_data *vd = (struct ap_vrprintf_data*)buff;

    if (vd->r->connection->aborted)
        return -1;

    /* r_flush is called when vbuff is completely full */
    if (buffer_output(vd->r, vd->buff, AP_IOBUFSIZE)) {
        return -1;
    }

    /* reset the buffer position */
    vd->vbuff.curpos = vd->buff;
    vd->vbuff.endpos = vd->buff + AP_IOBUFSIZE;

    return 0;
}