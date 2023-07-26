#!/usr/bin/perl
print " ~~~ welcome! ~~~\n";
$| = 1;
while (<>) {
   chomp $_;
   print "playing $_\n";
   my $pid = fork();
   if ($pid == 0) {
      exec("play -q $_.mp3");
   }
}
exit 0;
