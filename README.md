# B.Angr
A multi-dimensional dynamicly distorted staggered multi-bandpass LV2 plugin, for extreme soundmangling.
Based on Airwindows XRegion.

Key features:
* Multi-bandpass / distortion
* Cross-fading between four instances
* Automatic or user-controlled flow
* Optionally follows the beat

![screenshot](https://raw.githubusercontent.com/sjaehn/BAngr/master/doc/screenshot.png "Screenshot from B.Angr")


## Installation

a) Install the bangr package for your system
* [Arch](https://aur.archlinux.org/packages/bangr.lv2-git) by SpotlightKid
* [FreeBSD](https://www.freshports.org/audio/bangr-lv2) by yurivict

Note: This will NOT necessarily install the latest version of B.Angr. The version provided depends on the packagers.

b) Use the latest provided binaries

Unpack the provided bangr-\*.zip or bangr-\*.tar.xz from the latest release and 
copy the BAngr.lv2 folder to your lv2 directory (depending on your system settings,
~/.lv2/, /usr/lib/lv2/, /usr/local/lib/lv2/, or ...).

c) Build your own binaries in the following three steps.

Step 1: [Download the latest published version](https://github.com/sjaehn/BAngr/releases) of B.Angr. 
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

**Optional:** Standard `make` and `make install` parameters are supported. You may build a debugging version using
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

The flexible amount of speed and spin can be increased or decreased by dragging or scrolling
the white arrows on the outside the repective dial. The behaviour of the flexible amount can
be controlled by the audio input signal. Set the type (level, lows, mids, highs) and the amount 
of control (from 0 = random to 1 = full control). And let the white pont move to the music. 
Or leave everything in the random state.

More about XRegion at http://www.airwindows.com/xregion/ .


## Customize

You can create customized builds of B.Angr using the parameters `LANGUAGE` and `SKIN` (once
available). To create a new language pack, copy `src/Locale_EN.hpp` and edit
the text for the respective definitions. But do not change or delete any definition symbol!

To create a new skin, duplicate `src/Skin_Default.hpp`, rename it to `src/Skin_Default.hpp`
and edit the parameters.


## What's new

* Faster
  * Optimized DSP code
  * Compiler optimizations (`-O3 -ffast-math`)
* Support lv2:Parameters, lv2:State
* Cursor XY can now externally be controlled
* Binary compatibility improved
  * Static libs
  * Strip symbols by default
* Binary packages provided


## Acknowledgments

* Thanks to Chris Johnson for the Airwindows plugin XRegion: http://www.airwindows.com/xregion/ . 
Support his work via https://www.patreon.com/airwindows .


## Links

* Preview video: https://www.youtube.com/watch?v=-kWy_1UYazo
