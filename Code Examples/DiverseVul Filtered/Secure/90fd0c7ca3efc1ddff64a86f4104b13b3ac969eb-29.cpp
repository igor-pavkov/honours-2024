ENUM_PTRS_WITH(pdf14_device_enum_ptrs, pdf14_device *pdev)
{
    index -= 5;
    if (index < pdev->devn_params.separations.num_separations)
        ENUM_RETURN(pdev->devn_params.separations.names[index].data);
    index -= pdev->devn_params.separations.num_separations;
    if (index < pdev->devn_params.pdf14_separations.num_separations)
        ENUM_RETURN(pdev->devn_params.pdf14_separations.names[index].data);
    return 0;
}