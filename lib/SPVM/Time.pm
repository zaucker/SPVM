package SPVM::Time;

use SPVM 'SPVM::Time';

1;

=head1 NAME

SPVM::Time - Time manipulation

=head1 SYNOPSYS
  
  use SPVM::Time;
  
  # Epoch time
  my $time = SPVM::Time->time;
  
  # Local time information
  my $time_info = SPVM::Time->localtime(SPVM::Time->time);
  
  # GM time information
  my $time_info = SPVM::Time->gmtime(SPVM::Time->time);

=head1 DESCRIPTION

Time manipulation.

=head1 CLASS METHODS

=head2 time

  sub time : long ();

Returns the number of non-leap seconds since whatever time the system considers
to be the epoch, suitable for feeding to "gmtime" and "localtime".
On most systems the epoch is 00:00:00 UTC, January 1, 1970;

Example:

  my $time = SPVM::Time->time;

=head2 localtime

  sub localtime : SPVM::Time::Info ($time : long)

Converts a time as returned by the time function to a L<SPVM::Time::Info> object
with the time analyzed for the local time zone.

Example:

  my $time_info = SPVM::Time->localtime(SPVM::Time->time);

"mday" is the day of the month and "mon" the month in the range
0..11, with 0 indicating January and 11 indicating December.

"year" contains the number of years since 1900. To get a 4-digit
year write:

    my $year = $time_info->year + 1900;

"wday" is the day of the week, with 0 indicating Sunday and 3
indicating Wednesday. "yday" is the day of the year, in the range
0..364 (or 0..365 in leap years.)

"isdst" is true if the specified time occurs during Daylight Saving
Time, false otherwise.

If you get current time information, pass return value of "time" method.

  my $time_info = SPVM::Time->localtime(SPVM::Time->time)

See also the "timelocal" method. (for converting seconds,
minutes, hours, and such back to the integer value returned by
time()).

=head2 gmtime

  sub gmtime : SPVM::Time::Info ($time : long)

Works just like "localtime" but the returned values are localized
for the standard Greenwich time zone.

See also the "timegm" method. (for converting seconds,
minutes, hours, and such back to the integer value returned by
time()).

=head2 timelocal

  sub timelocal : long ($time_info : SPVM::Time::Info)

=head2 timegm

  sub timegm : long ($time_info : SPVM::Time::Info)

=head2 strptime

  sub strptime : SPVM::Time::Info ($str : string, $format : string)

=head2 strftime

  sub strftime : string ($format : string, $time_info : SPVM::Time::Info)

