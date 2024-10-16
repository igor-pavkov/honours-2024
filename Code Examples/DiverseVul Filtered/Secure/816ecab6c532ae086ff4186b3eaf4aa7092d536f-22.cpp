static void MngInfoDiscardObject(MngInfo *mng_info,int i)
{
  if (i && (i < MNG_MAX_OBJECTS) && (mng_info != (MngInfo *) NULL) &&
      mng_info->exists[i] && !mng_info->frozen[i])
    {
#ifdef MNG_OBJECT_BUFFERS
      if (mng_info->ob[i] != (MngBuffer *) NULL)
        {
          if (mng_info->ob[i]->reference_count > 0)
            mng_info->ob[i]->reference_count--;

          if (mng_info->ob[i]->reference_count == 0)
            {
              if (mng_info->ob[i]->image != (Image *) NULL)
                mng_info->ob[i]->image=DestroyImage(mng_info->ob[i]->image);

              mng_info->ob[i]=DestroyString(mng_info->ob[i]);
            }
        }
      mng_info->ob[i]=(MngBuffer *) NULL;
#endif
      mng_info->exists[i]=MagickFalse;
      mng_info->invisible[i]=MagickFalse;
      mng_info->viewable[i]=MagickFalse;
      mng_info->frozen[i]=MagickFalse;
      mng_info->x_off[i]=0;
      mng_info->y_off[i]=0;
      mng_info->object_clip[i].left=0;
      mng_info->object_clip[i].right=(ssize_t) PNG_UINT_31_MAX;
      mng_info->object_clip[i].top=0;
      mng_info->object_clip[i].bottom=(ssize_t) PNG_UINT_31_MAX;
    }
}