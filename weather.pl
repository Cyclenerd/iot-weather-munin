#!/usr/bin/perl

use strict;

###### Adjust the following storage location #########

# Storage location for weather files with a trailing slash to indicate a directory
my $store = '/var/lib/weather/';

###### Change the following only if you know what you're doing #########


=head1 NAME

weather.pl - CGI script to store celsius, humidity and pressure from sensors

=head1 APPLICABLE SYSTEMS

Should work with any system

=head1 CONFIGURATION

More details: https://github.com/Cyclenerd/iot-weather-munin

=head1 VERSION

v.0.1

=head1 AUTHOR

L<Nils Knieling|https://github.com/Cyclenerd>

=head1 LICENSE

GPLv3

=cut

# Check storage location
if ($store =~ /\/$/) {
    unless (-d $store) { mkdir($store, 0770) }
    unless (-d $store) { die( "Can\'t create $store: $!" ) }
} else {
    die 'You forgot the trailing slash to indicate a directory!';
}

# GET query string
my $q = $ENV{'QUERY_STRING'} || '';
# Parse and check params
my ($rw, $name, $celsius, $humidity, $pressure);
if ($q =~ /(w|r)=([\d\w]{3,15})/) {
    $rw = $1;
    $name   = $2;
}
if ($q =~ /c=([\d\-\.]*)/) {
    $celsius = $1;
}
if ($q =~ /h=([\d\.]*)/) {
    $humidity = $1;
}
if ($q =~ /p=([\d\.]*)/) {
    $pressure = $1;
}

print "HTTP/1.0 200 OK\n";
print "Content-Type: text/html\n\n";

# Check name
if ($name) {
    my $celsius_store  = "$store".'weather_'."$name".'_celsius.txt';
    my $humidity_store = "$store".'weather_'."$name".'_humidity.txt';
    my $pressure_store = "$store".'weather_'."$name".'_pressure.txt';
    if ($rw eq 'w') {
        # Write text file
        if (length $celsius) {
            # Save celsius
            if (open CELSIUS, "> $celsius_store") {
                print CELSIUS "$celsius";
                close CELSIUS;
            } else {
                print '<p>Can\'t save celsius';
            }
        }
        if (length $humidity) {
            # Save humidity
            if (open HUMIDITY, "> $humidity_store") {
                print HUMIDITY "$humidity";
                close HUMIDITY;
            } else {
                print '<p>Can\'t save humidity';
            }
        }
        if (length $pressure) {
            # Save pressure
            if (open PRESSURE, "> $pressure_store") {
                print PRESSURE "$pressure";
                close PRESSURE;
            } else {
                print '<p>Can\'t save pressure';
            }
        }
    } elsif ($rw eq 'r') {
        # Read text files and create html page
        my $title = $name.': ';
        my $text = '';
        if (open CELSIUS, "< $celsius_store") {
            $celsius = <CELSIUS>;
            close CELSIUS;
            if ($celsius =~ /^[\d\-\.]*$/) {
                $title .= "$celsius &deg;C";
                $text  .= "<h2>Temperature: $celsius &deg;C</h2>";
                $text  .= '<p><a href=/munin/localhost/localhost/weather_'.$name.'_celsius.html>';
                $text  .= '<img src=/munin/localhost/localhost/weather_'.$name.'_celsius-day.png></a>';
            }
        }
        if (open HUMIDITY, "< $humidity_store") {
            $humidity = <HUMIDITY>;
            close HUMIDITY;
            if ($humidity =~ /^[\d\.]*$/) {
                $title .= " / $humidity %";
                $text  .= "<h2>Humidity: $humidity %</h2>";
                $text  .= '<p><a href=/munin/localhost/localhost/weather_'.$name.'_humidity.html>';
                $text  .= '<img src=/munin/localhost/localhost/weather_'.$name.'_humidity-day.png></a>';
            }
        }
        if (open PRESSURE, "< $pressure_store") {
            $pressure = <PRESSURE>;
            close PRESSURE;
            if ($pressure =~ /^[\d\.]*$/) {
                $text  .= "<h2>Pressure: $pressure hPa</h2>";
                $text  .= '<p><a href=/munin/localhost/localhost/weather_'.$name.'_pressure.html>';
                $text  .= '<img src=/munin/localhost/localhost/weather_'.$name.'_pressure-day.png></a>';
            }
        }
        print '<meta content="width=device-width,initial-scale=1"name=viewport>';
        print '<meta content=360 http-equiv=refresh>';
        print '<link href=weather.png rel=icon type=image/png>';
        print '<style>body{font-family:sans-serif}img{width:370;display:block;margin:0 auto}</style>';
        print "<title>$title</title>";
        print "<body><h1>$name</h1>$text";
    } else {
        print "<p>Read or write?"
    }
} else {
    print '<p>Please provide a valid name (at least 3 characters up to 15 characters a-zA-Z0-9)';
}
