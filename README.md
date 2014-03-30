### ghost

The best audio player.

![ss](https://raw.githubusercontent.com/brooksbp/ghost/master/doc/2014-03-30_ss.png)

##### For Users

    $ ./ghost -help
    -help
    --dir=/path/to/library/             music library path.
    --pls=/path/to/playlist.pls         play/stream playlist.

##### For Developers

Install dependencies:

    apt-get install cmake libgstreamer1.0-dev gstreamer1.0-plugins-base gstreamer1.0-plugins-ugly qt5-default libgtk2.0-dev libevent-dev

    pacman -S cmake gstreamer gst-plugins-base gst-plugins-ugly qt5-base glib libmad

After clone, pull down submodules and apply any patches:

    ./init

Build:

    cd build
    cmake ..
    make
