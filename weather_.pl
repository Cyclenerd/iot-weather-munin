#!/usr/bin/perl

# -*- perl -*-

use strict;
use File::Basename;

###### Adjust the following storage location #########

# Storage location for weather files with a trailing slash to indicate a directory
my $store = '/var/lib/weather/';

###### Change the following only if you know what you're doing #########


=head1 NAME

weather_.pl - Munin plugin to monitor celsius, humidity and pressure from sensors

=head1 APPLICABLE SYSTEMS

Should work with any system

=head1 CONFIGURATION

More details: https://github.com/Cyclenerd/iot-weather-munin

=head1 MAGIC MARKERS

#%# family=manual

=head1 VERSION

v.0.1

=head1 AUTHOR

L<Nils Knieling|https://github.com/Cyclenerd>

=head1 LICENSE

GPLv3

=cut


my $file_name = basename($0);

if ($file_name =~ /^weather_([\d\w]{3,15})_(celsius|humidity|pressure)$/) {
    my $name = $1;
    my $what = $2;
    # Celsius
    if ($what eq 'celsius') {
        if (defined $ARGV[0] and $ARGV[0] eq "config") {
            print "graph_title $name temperature\n";
            print "graph_vlabel degrees Celsius\n";
            print "graph_category sensors\n";
            print 'weather_'."$name".'_celsius.label '."Temperature\n";
            exit 0;
        }
        my $celsius_store = "$store".'weather_'."$name".'_celsius.txt';
        if (open CELSIUS, "< $celsius_store") {
            my $celsius = <CELSIUS>;
            close CELSIUS;
            if ($celsius =~ /^[\d\-\.]*$/) {
                print 'weather_'."$name".'_celsius.value '."$celsius\n";
            }
        } else {
            die 'Can\'t read celsius';
        }
    # Humidity
    } elsif ($what eq 'humidity') {
        if (defined $ARGV[0] and $ARGV[0] eq "config") {
            print "graph_title $name humidity\n";
            print "graph_vlabel %\n";
            print "graph_category sensors\n";
            print 'weather_'."$name".'_humidity.label '."Humidity\n";
            exit 0;
        }
        my $humidity_store = "$store".'weather_'."$name".'_humidity.txt';
        if (open HUMIDITY, "< $humidity_store") {
            my $humidity = <HUMIDITY>;
            close HUMIDITY;
            if ($humidity =~ /^[\d\.]*$/) {
                print 'weather_'."$name".'_humidity.value '."$humidity\n";
            }
        } else {
            die 'Can\'t read humidity';
        }
    # Pressure
    } elsif ($what eq 'pressure') {
        if (defined $ARGV[0] and $ARGV[0] eq "config") {
            print "graph_title $name pressure\n";
            print "graph_vlabel hPa\n";
            print "graph_category sensors\n";
            print 'weather_'."$name".'_pressure.label '."Pressure\n";
            exit 0;
        }
        my $pressure_store = "$store".'weather_'."$name".'_pressure.txt';
        if (open PRESSURE, "< $pressure_store") {
            my $pressure = <PRESSURE>;
            close PRESSURE;
            if ($pressure =~ /^[\d\.]*$/) {
                print 'weather_'."$name".'_pressure.value '."$pressure\n";
            }
        } else {
            die 'Can\'t read pressure';
        }
    } else {
        die "Neither celsius, humidity nor pressure";
    }
} else {
    die "Please use a valid filename: weather_<NAME>_celsius, weather_<NAME>_humidity or weather_<NAME>_pressure";
}
