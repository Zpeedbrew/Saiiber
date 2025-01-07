# Saiiber
Wii homebrew clone of Beat Saber (IN PROGRESS CURRENTLY)

Discord Server: https://dsc.gg/saiiber
Wiibrew Page: http://wiibrew.org/wiki/User:Zpeedbrew/Saiiber

# FAQ:

#compiling 
--add something about devkitpro
needed lirarys

Is this project alive?<br>
Yes, it will be developed guinea7pig

Is this real?<br>
It is. We're not trolling around. This is an actual project that's going to be developed.

Is this available?<br>
As of right now, no, it's currently being developed.

How can I get updates about the project?<br>
Join the Discord server, go to #opt-in-for-announcement-pings and select the 🛠️ emoji to get the Dev-Opt role.


# Developer Notes
- Logger should log to the Dolphin console if you turn off "Enable MMU" in the advanced settings.
- sd directory contains the virtual SD card data.
- Libraries required are all installed with pacman and listed in the Makefile
- Makefile output was changed to output a "boot.dol/elf" in the sd/apps/saiiber directory

The virtual SD card on Dolphin can convert the folder into a file for use in the emulator. I wanted the SD directory to reflect the final product as it would be installed by any user.
