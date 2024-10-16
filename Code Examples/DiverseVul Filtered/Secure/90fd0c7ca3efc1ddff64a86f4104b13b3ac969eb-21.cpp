static	RELOC_PTRS_WITH(pdf14_device_reloc_ptrs, pdf14_device *pdev)
{
    {
        int i;

        for (i = 0; i < pdev->devn_params.separations.num_separations; ++i) {
            RELOC_PTR(pdf14_device, devn_params.separations.names[i].data);
        }
    }
    RELOC_VAR(pdev->ctx);
    RELOC_VAR(pdev->smaskcolor);
    RELOC_VAR(pdev->trans_group_parent_cmap_procs);
    pdev->target = gx_device_reloc_ptr(pdev->target, gcst);
    pdev->pclist_device = gx_device_reloc_ptr(pdev->pclist_device, gcst);
}