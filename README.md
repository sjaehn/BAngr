# B.Angr
A multi-dimensional dynamicly distorted staggered multi-bandpass LV2 plugin, for extreme soundmangling.
Based on Airwindows XRegion.

Key features:
* Multi-bandpass / distortion
* Cross-fading between four instances
* Automatic or user-controlled

![screenshot](https://raw.githubusercontent.com/sjaehn/BAngr/master/doc/screenshot.png "Screenshot from B.Angr")


## Installation

a) Install the bangr package for your system (once available)

b) Build your own binaries in the following three steps.

Step 1: [Download the latest published version](https://github.com/sjaehn/BAngr/releases - once established) of B.Angr. 
Or clone or [download the master](https://github.com/sjaehn/BAngr/archive/master.zip) of this repository.

Step 2: Install pkg-config and the development packages for x11, cairo, and lv2 if not done yet. If you
don't have already got the build tools (compilers, make, libraries) then install them too.

On Debian-based systems you may run:
```
sudo apt-get install build-essential
sudo apt-get install pkg-config libx11-dev libcairo2-dev lv2-dev
```

On Arch-based systems you may run:
```
sudo pacman -S base-devel
sudo pacman -S pkg-config libx11 cairo lv2
```

Step 3: Building and installing into the default lv2 directory (/usr/local/lib/lv2/) is easy using `make` and
`make install`. Simply call:
```
make
sudo make install
```

**Optional:** Standard `make` and `make install` parameters are supported. Compiling using `make CPPFLAGS+=-O3`
is recommended to improve the plugin performance. Alternatively, you may build a debugging version using
`make CPPFLAGS+=-g`. For installation into an alternative directory (e.g., /usr/lib/lv2/), change the
variable `PREFIX` while installing: `sudo make install PREFIX=/usr`. If you want to freely choose the
install target directory, change the variable `LV2DIR` (e.g., `make install LV2DIR=~/.lv2`) or even define
`DESTDIR`.

**Optional:** Further supported parameters are `LANGUAGE` (two letters code) to change the GUI language and
`SKIN` to change the skin (see customize).


## Running

After the installation Ardour, Carla, and any other LV2 host should automatically detect B.Angr.

If jalv is installed, you can also call it

```
jalv.gtk https://www.jahnichen.de/plugins/lv2/BAngr
```

to run it stand-alone and connect it to the JACK system.


## Usage

The user interface of B.Angr contains four parameter instances of the XRegion parameters gain, 
first, last, nuke, and mix. Plus an additional pan dial for each instance. Sliding between the
instances is controlled by the white point in the central controller fiels. The movement of
this point can be controlled by the parameters speed and spin. But you can also slide between 
the four instances by dragging the white point of the central controller field to
the respective instance.

More about XRegion at http://www.airwindows.com/xregion/ .


## Customize

You can create customized builds of B.Angr using the parameters `LANGUAGE` and `SKIN` (once
available). To create a new language pack, copy `src/Locale_EN.hpp` and edit
the text for the respective definitions. But do not change or delete any definition symbol!

To create a new skin, duplicate `src/Skin_Default.hpp`, rename it to `src/Skin_Default.hpp`
and edit the parameters. You can also provide a background image at `./surface_SkinName.png`.


## What's new

* This plugin
  

## TODO

* Code optimization / speed


## Acknowledgments

* Thanks to Chris Johnson for the Airwindows plugin XRegion: http://www.airwindows.com/xregion/


## Links

* Preview video: https://www.youtube.com/watch?v=-kWy_1UYazo
