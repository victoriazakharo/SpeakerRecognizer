#!/usr/bin/env perl

if (@ARGV != 1) {
  print "Usage: $0 <model-folder>";
  exit 1;
}

$modelFolder = shift @ARGV;
$tmpModelFile = "$modelFolder/tmp_model.txt";
$modelFile = "$modelFolder/model.txt";

open $input, $tmpModelFile or die "Could not open $tmpModelFile: $!";
while($line = <$input>) {
    chomp($line);
    @words = split / /, $line;  
    $size = @words[0];  
    $dictorsNum += $size;
    for ($i=0; $i < $size; $i++) {
	push @dictors, @words[$i*2+1]; 
	push @dictorSizes, @words[$i*2+2]; 
    }     
}
close $input;
open $output, ">$modelFile" or die "Could not open '$modelFile' $!";
print $output $dictorsNum;
for ($i=0; $i < $dictorsNum; $i++) {
	print $output " ".@dictors[$i]; 
	print $output " ".@dictorSizes[$i]; 
    }  
close $output;
