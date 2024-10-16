static void vhost_vdpa_clean_irq(struct vhost_vdpa *v)
{
	int i;

	for (i = 0; i < v->nvqs; i++)
		vhost_vdpa_unsetup_vq_irq(v, i);
}