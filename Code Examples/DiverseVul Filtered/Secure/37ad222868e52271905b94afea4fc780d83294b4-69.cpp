void client_fflush(void)
{
    if (replybuf_pos == replybuf) {
        return;
    }
    safe_write(clientfd, replybuf, (size_t) (replybuf_pos - replybuf), -1);
    client_init_reply_buf();
}