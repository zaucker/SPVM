package SPVM::std;

1;

=head1 NAME

SPVM::std - Default Libraray

=head1 FUNCTIONS

=head2 say

Print string with line break to stdout.

  sub say ($value : byte[]) : native void;

=head2 say_byte

Print byte value with line break to stdout.

  sub say_byte ($value : byte) : native void;

=head2 say_short

Print short value with line break to stdout.

  sub say_short ($value : short) : native void;

=head2 say_int

Print int value with line break to stdout.

  sub say_int ($value : int) : native void;

=head2 say_long

  sub say_long ($value : long) : native void;

=head2 say_float

Print float value with line break to stdout.

  sub say_float ($value : float) : native void;

=head2 say_double

Print double value with line break to stdout.

  sub say_double ($value : double) : native void;

=head2 print

Print string to stdout.

  sub print ($value : byte) : native void;

=head2 print_byte

Print byte value to stdout.

  sub print_byte ($value : byte) : native void;

=head2 print_short

Print short value to stdout.

  sub print_short ($value : short) : native void;

=head2 print_int

Print int value to stdout.

  sub print_int ($value : int) : native void;

=head2 print_long

Print long value to stdout.

  sub print_long ($value : long) : native void;

=head2 print_float

Print float value to stdout.

  sub print_float ($value : float) : native void;

=head2 print_double

Print double value to stdout.

  sub print_double ($value : double) : native void;

=head2 say_err

Print string with line break to stderr.

  sub say_err ($value : byte[]) : native void;

=head2 say_err_byte

Print byte value with line break to stderr.

  sub say_err_byte ($value : byte) : native void;

=head2 say_err_short

Print short value with line break to stderr.

  sub say_err_short ($value : short) : native void;

=head2 say_err_int

Print int value with line break to stderr.

  sub say_err_int ($value : int) : native void;

=head2 say_err_long

Print long value with line break to stderr.

  sub say_err_long ($value : long) : native void;

=head2 say_err_float

Print float value with line break to stderr.

  sub say_err_float ($value : float) : native void;

=head2 say_err_double

Print double value with line break to stderr.

  sub say_err_double ($value : double) : native void;

=head2 print_err

Print string to stderr.

  sub print_err ($value : byte[]) : native void;

=head2 print_err_byte

Print byte value to stderr.

  sub print_err_byte ($value : byte) : native void;

=head2 print_err_short

Print short value to stderr.

  sub print_err_short ($value : short) : native void;

=head2 print_err_int

Print int value to stderr.

  sub print_err_int ($value : int) : native void;

=head2 print_err_long

Print long value to stderr.

  sub print_err_long ($value : long) : native void;

=head2 print_err_float

Print float value to stderr.

  sub print_err_float ($value : float) : native void;

=head2 print_err_double

Print double value to stderr.

  sub print_err_double ($value : double) : native void;

=head2 time

Get epoch time.

  sub time () : native long;
