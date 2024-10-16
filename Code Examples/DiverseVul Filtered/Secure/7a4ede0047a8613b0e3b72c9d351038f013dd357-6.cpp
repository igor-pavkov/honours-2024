static void oss_dump_info (struct oss_params *req, struct oss_params *obt)
{
    dolog ("parameter | requested value | obtained value\n");
    dolog ("format    |      %10d |     %10d\n", req->fmt, obt->fmt);
    dolog ("channels  |      %10d |     %10d\n",
           req->nchannels, obt->nchannels);
    dolog ("frequency |      %10d |     %10d\n", req->freq, obt->freq);
    dolog ("nfrags    |      %10d |     %10d\n", req->nfrags, obt->nfrags);
    dolog ("fragsize  |      %10d |     %10d\n",
           req->fragsize, obt->fragsize);
}