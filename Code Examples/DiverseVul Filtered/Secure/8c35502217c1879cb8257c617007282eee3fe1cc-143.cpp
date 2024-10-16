void Magick::Image::convolve(const size_t order_,const double *kernel_)
{
  KernelInfo
    *kernel_info;

  GetPPException;
  kernel_info=AcquireKernelInfo((const char *) NULL,exceptionInfo);
  kernel_info->width=order_;
  kernel_info->height=order_;
  kernel_info->x=(ssize_t) (order_-1)/2;
  kernel_info->y=(ssize_t) (order_-1)/2;
  kernel_info->values=(MagickRealType *) AcquireAlignedMemory(order_,
    order_*sizeof(*kernel_info->values));
  if (kernel_info->values != (MagickRealType *) NULL)
    {
      MagickCore::Image
        *newImage;

      for (ssize_t i=0; i < (ssize_t) (order_*order_); i++)
        kernel_info->values[i]=kernel_[i];
      newImage=ConvolveImage(image(),kernel_info,exceptionInfo);
      replaceImage(newImage);
    }
  kernel_info=DestroyKernelInfo(kernel_info);
  ThrowImageException;
}