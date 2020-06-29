use lib "t/lib";
use TestAuto;

use strict;
use warnings;

use Test::More 'no_plan';

use SPVM 'TestCase::Isa';

UNITCHECK { SPVM::init() }

# Start objects count
my $start_memory_blocks_count = SPVM::get_memory_blocks_count();

# isa
{
  ok(TestCase::Isa->isa_any_object_type());
  ok(TestCase::Isa->isa_numeric_type());
  ok(TestCase::Isa->isa_multi_numeric_type());
  ok(TestCase::Isa->isa_match_class());
  ok(TestCase::Isa->isa_not_match_class());
  ok(TestCase::Isa->isa_match_array());
  ok(TestCase::Isa->isa_not_match_class_undef());
  ok(TestCase::Isa->isa_match_callback());
  ok(TestCase::Isa->isa_not_match_callback_undef());
}

# All object is freed
my $end_memory_blocks_count = SPVM::get_memory_blocks_count();
is($end_memory_blocks_count, $start_memory_blocks_count);
