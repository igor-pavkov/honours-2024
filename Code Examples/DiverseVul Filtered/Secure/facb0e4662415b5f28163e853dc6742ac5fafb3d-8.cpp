static CURLcode tftp_doing(struct connectdata *conn, bool *dophase_done)
{
  CURLcode result;
  result = tftp_multi_statemach(conn, dophase_done);

  if(*dophase_done) {
    DEBUGF(infof(conn->data, "DO phase is complete\n"));
  }
  else if(!result) {
    /* The multi code doesn't have this logic for the DOING state so we
       provide it for TFTP since it may do the entire transfer in this
       state. */
    if(Curl_pgrsUpdate(conn))
      result = CURLE_ABORTED_BY_CALLBACK;
    else
      result = Curl_speedcheck(conn->data, Curl_now());
  }
  return result;
}