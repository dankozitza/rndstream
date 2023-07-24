```

RNDSTREAM

   Rndstream generates customized random text.

Usage:

   rndstream [command] [-options]

Options:

   -s <seed>   Set the seed passed to srand.
   -w <width>  Set the number of characters 
               printed per line.
   -l <lines>  Set the number of lines printed per
               frame.
   -p <frames> Set the number of frames to print.
   -f <frame>  Set the last frame to be printed.
   -i <ignore> Set the number of frames to skip.
   -t <delay>  Set the time to wait between frames
               (seconds).
   -o <output> Define the output as a range of 
               characters.
               Ex: ' ~' for all, 'az' for 
               alphabet, '09' for digits.
   -O <list>   Define the output as a list of 
               characters.
               Ex: '123abc', '1123', '\/|-    '.
   -S <string> Define the output as a comma 
               separated list of strings.
               Ex: '70 ,80 ,90 ', 'G ,G ,C ,Em,Am,D '.
   -N <string> Define recursion triggers and their
               corresponding configuration files as
               a comma separated list.
               Ex: 'n,numbers.json,7,letters.json'
   -x          Set width and lines to terminal 
               size.
   -c <config> Set a custom config file location.
               (set every time)
   -r          Re-set the seed using current 
               time.
   -R          Re-set the seed using current 
               seed.
   -d          Do not overwrite the config file.
   -C          Exit rather than pause on Ctrl+c signal.
               (toggle)
   -n          Go to the next frame.
   -b          Go to the last frame.

Commands:

   env   - Display the configuration file.
   gen   - Generate random text. Use 'gen help' 
           for more info.

Use "./rndstream help [command]" for more 
information about a command.

Files:

   $HOME/.rndstream.json is the default configuration
   file and it must exist for rndstream to operate
   correctly. To override the use of this config file
   use the -c option or manually change the 'config'
   option in .rndstream.json.

Dependencies:

    Linux:  jsoncpp
    Debian: libjsoncpp-dev

    For debian you will also have to modify tools.hpp
    to use the proper #include directive.

Build:

   make

Install:

   sudo make install

Uninstall:

   sudo make uninstall

Cleanup:

   make clean

Notes:

   Termux requires a package called termux-elf-cleaner
   in order to stop an annoying linker warning.

      pkg install termux-elf-cleaner

   Then call

      termux-elf-cleaner rndstream

   after you call make.

```
