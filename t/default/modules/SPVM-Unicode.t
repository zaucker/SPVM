use lib "t/lib";
use TestAuto;

use strict;
use warnings;

use Test::More 'no_plan';

use SPVM 'TestCase::Lib::SPVM::Unicode';

# Start objects count
my $start_memory_blocks_count = SPVM::memory_blocks_count();

{
  ok(TestCase::Lib::SPVM::Unicode->uchar);
  ok(TestCase::Lib::SPVM::Unicode->uchar_to_u8);
  ok(TestCase::Lib::SPVM::Unicode->convert_u32_to_u16);
  ok(TestCase::Lib::SPVM::Unicode->convert_u16_to_u32);
  ok(TestCase::Lib::SPVM::Unicode->u16);
  ok(TestCase::Lib::SPVM::Unicode->u8);
}

# All object is freed
my $end_memory_blocks_count = SPVM::memory_blocks_count();
is($end_memory_blocks_count, $start_memory_blocks_count);