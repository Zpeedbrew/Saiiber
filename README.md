
# Saiiber
Wii homebrew clone of Beat Saber (IN PROGRESS CURRENTLY)

Discord Server: https://dsc.gg/saiiber
Wiibrew Page: http://wiibrew.org/wiki/User:Zpeedbrew/Saiiber

# FAQ:

Is this project alive?	

Yes, it will be developed by 
Guinea7pig.



Is this real?

It is. We're not trolling around. This is an actual project that's going to be developed.

Is this available?

yes as beta tests, it's currently being developed.

How can I get updates about the project?

Join the Discord server, go to #opt-in-for-announcement-pings and select the 🛠️ emoji to get the Dev-Opt role.

# Compiling:

- Install devkitPPC (if you are on linux install wii-dev with dkp-pacman)<!--blue-->
- Install the dependenices in the dependencies.txt file  <!--red-->
- Compile it using make  <!--blue-->
- launch the .elf file dolphin  <!--red-->

# Developer Notes
Uses clang-format with Google formatting. All pull requests must comply with these formatting requirements.

- Logger should log to the Dolphin console if you turn off "Enable MMU" in the advanced settings.
- sd directory contains the virtual SD card data.
- Libraries required are all installed with pacman and listed in the Makefile
- Makefile output was changed to output a "boot.dol/elf" in the sd/apps/saiiber directory

The virtual sd card on Dolphin can convert the folder into a file for use in the emulator. I wanted the sd directory to reflect the final product as it would be installed by any user.
