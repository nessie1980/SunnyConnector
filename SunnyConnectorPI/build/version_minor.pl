#! /usr/bin/perl -w

###################################################################################################
#
# Diese Script dient Erstellung einer MINOR-Version
#
###################################################################################################

use strict;
use warnings;

my $FileVersion = "../src/version.h";
my $FileDoxygen = "../Doxygen/SunnyConnectorPI.doxyfile";
my $NewContentVersion;
my $NewContentDoxygen;

my $BuildFlag = 0;
my $MinorFlag = 0;

my $NewVersionString = "";
my $BuildValue = 0;
my $MinorValue = 0;
my $MajorValue = 0;

my $EXIT_VERSION_ERROR = 1;
my $EXIT_DOXYGEN_ERROR = 2;

if ($ARGV[0])
{
 ($FileVersion) = @ARGV;
}
 
open (FileHandle, "<", $FileVersion) or die exit $EXIT_VERSION_ERROR;
while(<FileHandle>)
{
	my @array;
	my $Zeile = $_;

    if ($Zeile =~ /^#define BUILD/)
    {
    	my @array = split (/ / ,$Zeile);
		$array[$#array] = 1;
		$BuildValue = $array[$#array];
        $NewContentVersion = $NewContentVersion.join(" ", @array)."\n";
    }
    elsif ($Zeile =~ /^#define MINOR/)
    {
        @array = split (/ / ,$Zeile);
        if ($array[$#array] < 99)
        {
        	$array[$#array]++;
        }       
        else
        {
            $array[$#array] = 1;
			$MinorFlag = 1;
        }
		$MinorValue = $array[$#array];
        $NewContentVersion = $NewContentVersion.join(" ", @array)."\n";
	}	
    elsif ($Zeile =~ /^#define MAJOR/ && $MinorFlag == 1)
    {
		@array = split (/ / ,$Zeile);
		$array[$#array]++;
		$MajorValue = $array[$#array];
		$NewContentVersion = $NewContentVersion.join(" ", @array)."\n"; 
	}
	else
	{
		if ($Zeile =~ /^#define MAJOR/)
		{
			@array = split (/ / ,$Zeile);
			$MajorValue = $array[$#array] + 0;
		}
		$NewContentVersion = $NewContentVersion.$Zeile;
	}
}

$NewVersionString = sprintf("%d.%02d.%03d", $MajorValue, $MinorValue, $BuildValue);

close(FileHandle);
open (FileHandle, ">", $FileVersion) or die exit $EXIT_VERSION_ERROR;
print FileHandle $NewContentVersion;
close(FileHandle);

open (FileHandle, "<", $FileDoxygen) or die exit $EXIT_DOXYGEN_ERROR;
while(<FileHandle>)
{
	my @array;
	my $Zeile = $_;

    if ($Zeile =~ /^PROJECT_NUMBER         = "Version /)
    {
    	$NewContentDoxygen = $NewContentDoxygen.join("", "PROJECT_NUMBER         = \"Version ", $NewVersionString, "\"")."\n";
	}
	else
	{
		$NewContentDoxygen = $NewContentDoxygen.$Zeile;
	}
}
close(FileHandle);
open (FileHandle, ">", $FileDoxygen) or die exit $EXIT_DOXYGEN_ERROR;
print FileHandle $NewContentDoxygen;
close(FileHandle);

exit 0;
