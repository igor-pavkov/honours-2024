DeepTiledInputFile::readTiles (int dx1, int dx2, int dy1, int dy2, int lx, int ly)
{
    //
    // Read a range of tiles from the file into the framebuffer
    //

    try
    {
#if ILMBASE_THREADING_ENABLED
        std::lock_guard<std::mutex> lock (*_data->_streamData);
#endif
        if (_data->slices.size() == 0)
            throw IEX_NAMESPACE::ArgExc ("No frame buffer specified "
                               "as pixel data destination.");

        if (!isValidLevel (lx, ly))
            THROW (IEX_NAMESPACE::ArgExc,
                   "Level coordinate "
                   "(" << lx << ", " << ly << ") "
                   "is invalid.");

        //
        // Determine the first and last tile coordinates in both dimensions.
        // We always attempt to read the range of tiles in the order that
        // they are stored in the file.
        //

        if (dx1 > dx2)
            std::swap (dx1, dx2);

        if (dy1 > dy2)
            std::swap (dy1, dy2);

        int dyStart = dy1;
        int dyStop  = dy2 + 1;
        int dY      = 1;

        if (_data->lineOrder == DECREASING_Y)
        {
            dyStart = dy2;
            dyStop  = dy1 - 1;
            dY      = -1;
        }

        //
        // Create a task group for all tile buffer tasks.  When the
        // task group goes out of scope, the destructor waits until
        // all tasks are complete.
        //

        {
            TaskGroup taskGroup;
            int tileNumber = 0;

            for (int dy = dyStart; dy != dyStop; dy += dY)
            {
                for (int dx = dx1; dx <= dx2; dx++)
                {
                    if (!isValidTile (dx, dy, lx, ly))
                        THROW (IEX_NAMESPACE::ArgExc,
                               "Tile (" << dx << ", " << dy << ", " <<
                               lx << "," << ly << ") is not a valid tile.");

                    ThreadPool::addGlobalTask (newTileBufferTask (&taskGroup,
                                                                  _data,
                                                                  tileNumber++,
                                                                  dx, dy,
                                                                  lx, ly));
                }
            }

            //
            // finish all tasks
            //
        }

        //
        // Exeption handling:
        //
        // TileBufferTask::execute() may have encountered exceptions, but
        // those exceptions occurred in another thread, not in the thread
        // that is executing this call to TiledInputFile::readTiles().
        // TileBufferTask::execute() has caught all exceptions and stored
        // the exceptions' what() strings in the tile buffers.
        // Now we check if any tile buffer contains a stored exception; if
        // this is the case then we re-throw the exception in this thread.
        // (It is possible that multiple tile buffers contain stored
        // exceptions.  We re-throw the first exception we find and
        // ignore all others.)
        //

        const string *exception = 0;

        for (size_t i = 0; i < _data->tileBuffers.size(); ++i)
        {
            TileBuffer *tileBuffer = _data->tileBuffers[i];

            if (tileBuffer->hasException && !exception)
                exception = &tileBuffer->exception;

            tileBuffer->hasException = false;
        }

        if (exception)
            throw IEX_NAMESPACE::IoExc (*exception);
    }
    catch (IEX_NAMESPACE::BaseExc &e)
    {
        REPLACE_EXC (e, "Error reading pixel data from image "
                     "file \"" << fileName() << "\". " << e.what());
        throw;
    }
}