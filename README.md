___
WARNING: UNDER CONSTRUCTION
___

### ghost

The best audio player.

![ss](https://raw.githubusercontent.com/brooksbp/ghost/master/doc/2014-03-30_ss.png)

##### For Users

    $ ./ghost -h
        -help, -h
    
        --library-dir=
        Path to directiory that contains all your audio files.
    
        --user-data-dir=
        Path to directiory that is used to store user data.
    

`pwd`/Preferences - preferences file in JSON format. Don't modify while app is running!
`pwd`/ghost.log - log file.

##### For Developers

Install dependencies:

    apt-get install cmake libgstreamer1.0-dev gstreamer1.0-plugins-base \
            gstreamer1.0-plugins-ugly qt5-default libgtk2.0-dev libevent-dev

    pacman -S cmake gstreamer gst-plugins-base gst-plugins-ugly qt5-base glib libmad

After clone, pull down submodules and apply any patches:

    ./init

Build:

    cd build
    cmake ..
    make
