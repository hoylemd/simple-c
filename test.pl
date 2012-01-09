#!/usr/bin/perl

# file: test.pl
print "hey there\n";

$name = "Mike " . "Hoyle";

%hashexample = ( "Unrelenting Force" => "Fus Ro Dah", "Fire Breath" => "Yol Tor Shul" );

print "your name is ", $name, "\n";

$shout = "Fire Breath";

print "The words of power for $shout are: " . $hashexample{$shout} . "\n";

@arrayexample = (1 .. 10);
$limit = 27;

for $i (@arrayexample, "fifteen", 20 .. $limit) {
    print "$i\n";
}

print "shouts!\n";

for $i (keys %hashexample )
{
    print "The words of power for $i are $hashexample{$i}\n";
}

