static int nntp_msg_close(struct Context *ctx, struct Message *msg)
{
  return mutt_file_fclose(&msg->fp);
}