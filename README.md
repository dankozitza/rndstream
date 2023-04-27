```

rndstream

   Rndstream generates customized random text.

Usage:

   rndstream [command] [-options]

Options:

   -s <seed>   Set the seed passed to srand.
   -w <width>  Set the number of characters printed per line.
   -l <lines>  Set the number of lines printed per frame.
   -p <print>  Set the number of frames to print.
   -f <frames> Set the last frame to be printed.
   -i <ignore> Set the number of frames to skip.
   -t <delay>  Set the time to wait between frames (seconds).
   -o <output> Define the output characters. Ex: ' ~' for all,
               'az' for lowercase alphabet, '09' for digits.
   -O <list>   Define the output as a list of characters.
               Ex: '123abc', '1123', '\/|-    '.
   -c <config> Set a custom config file location. (set every time)
   -r          Re-set the seed using the current time.
   -R          Re-set the seed using the current seed.
   -d          Do not overwrite the config file.
   -x          Set width and lines to terminal size.

Commands:

   env        - Displays the configuration file.
   gen        - Generate random text. Use 'gen help' for more info.

Use "./rndstream help [command]" for more information about a command.

Files:

   $HOME/.rndstream.json is the default configuration file and it must exist
   for rndstream to operate correctly. To override the use of this config
   file use the -c option or manually change the 'config' option in
   .rndstream.json.

Build:

   make

Install:

   sudo make install

Uninstall:

   sudo make uninstall

Cleanup:

   make clean

```
