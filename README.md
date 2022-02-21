```
rndstream

   Rndstream generates customized random text.

Usage:

   rndstream <command> [arguments]

Options:

   -s <seed>   Set the seed passed to srand.
   -w <width>  Set the number of characters printed per line.
   -l <lines>  Set the number of lines printed per frame.
   -f <frames> Set the number of frames printed.
   -t <delay>  Set the time to wait between frames (seconds).
   -o <output> Define the output characters. Ex: ' ~' for all,
               'az' for lowercase alphabet, '09' for digits.
   -c <config> Set a custom config file location. (set every time)
   -r          Re-set the seed using current time.
   -x          Set width and lines to terminal size.

Commands:

   env        - Displays the configuration file.
   gen        - Generate random text. Use 'gen help' for more info.

Use "./rndstream help [command]" for more information about a command.
```
