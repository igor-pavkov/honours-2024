#endif
    }

    //! Load a multi-page TIFF file \newinstance.
    static CImgList<T> get_load_tiff(const char *const filename,
				     const unsigned int first_frame=0, const unsigned int last_frame=~0U,
				     const unsigned int step_frame=1,