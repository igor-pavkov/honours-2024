void vidtv_s302m_encoder_destroy(struct vidtv_encoder *e)
{
	if (e->id != S302M) {
		pr_err_ratelimited("Encoder type mismatch, skipping.\n");
		return;
	}

	vidtv_s302m_access_unit_destroy(e);
	kfree(e->name);
	vfree(e->encoder_buf);
	kfree(e->ctx);
	kfree(e);
}