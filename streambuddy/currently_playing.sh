#!/bin/bash

FILENAME="`audtool current-song-filename`"
id3socialtool z "${FILENAME}" 2>&1



