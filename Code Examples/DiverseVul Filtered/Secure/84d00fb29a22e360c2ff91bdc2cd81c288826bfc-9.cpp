static void jpc_streamlist_destroy(jpc_streamlist_t *streamlist)
{
	int streamno;
	if (streamlist->streams) {
		for (streamno = 0; streamno < streamlist->numstreams;
		  ++streamno) {
			jas_stream_close(streamlist->streams[streamno]);
		}
		jas_free(streamlist->streams);
	}
	jas_free(streamlist);
}