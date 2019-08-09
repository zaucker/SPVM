package SPVM::BlessedObject::Array;

use base 'SPVM::BlessedObject';

use SPVM::ExchangeAPI;

use overload bool => sub {1}, '""' => sub { shift->to_str }, fallback => 1;

sub to_elems {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::to_elems($env, $self);
}

sub to_bin {
  my $self = shift;

  my $env = $self->{env};
  
  SPVM::ExchangeAPI::to_bin($env, $self);
}

sub to_str {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::to_str($env, $self);
}

sub to_strs {
  my $self = shift;
  
  my $env = $self->{env};
  
  SPVM::ExchangeAPI::to_strs($env, $self);
}

1;

=head1 NAME

SPVM::BlessedObject::Array - Array based blessed object

=head2 DESCRIPTION

SPVM::BlessedObject::Array is array based blessed object.

This object contains SPVM array object.

=head1 SYNOPSYS
  
  # Convert SPVM array to Perl array reference
  my $nums = $spvm_nums->to_elems;

  # Convert SPVM array to Perl binary data
  my $binary = $spvm_nums->to_bin;
  
  # Convert SPVM array to perl text str(decoded str).
  my $str = $spvm_str->to_str;

  # Convert SPVM array to perl array reference which contains decoded strings.
  my $strs = $spvm_strs->to_strs;

=head1 METHODS

=head2 to_elems

  my $nums = $spvm_nums->to_elems;

Convert SPVM array to Perl array reference.

=head2 to_bin

  my $binary = $spvm_nums->to_bin;

Convert SPVM array to binary data.

Binary data is unpacked by C<unpack> function.

An exmaple when array is int array:

  my @nums = unpack 'l*', $binary;

=head2 to_str

  my $str = $spvm_str->to_str;

Convert SPVM array to perl text str(decoded str).

=head2 to_strs

  my $strs = $spvm_strs->to_strs;

Convert SPVM array to perl array reference which contains decoded strings.