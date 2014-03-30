### ghost

The best audio player. Ever.

    $ ./ghost -help
    -help
    --dir=/path/to/library/             music library path.
    --pls=/path/to/playlist.pls         play/stream playlist.

##### Build

    ./init
    
    cd build

Linux:

    cmake ..
    make

##### Platform Dependencies

Ubuntu / Debian:

    apt-get install cmake libgstreamer1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-ugly qt5-default libgtk2.0-dev libevent-dev

Arch Linux:

    pacman -S cmake gstreamer gst-plugins-base gst-plugins-ugly qt5-base glib libmad

