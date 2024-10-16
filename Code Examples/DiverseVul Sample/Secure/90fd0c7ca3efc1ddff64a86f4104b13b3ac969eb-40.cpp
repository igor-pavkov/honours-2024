static pdf14_device *find_pdf14_device(gx_device *dev)
{
    pdf14_device *pdev;

    if (dev_proc(dev, dev_spec_op)(dev, gxdso_is_pdf14_device, &pdev, sizeof(pdev)) <= 0)
        return NULL;
    return pdev;
}