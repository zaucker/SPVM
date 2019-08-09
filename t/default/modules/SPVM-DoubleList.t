use lib "t/lib";
use TestAuto;

use strict;
use warnings;

use Test::More 'no_plan';

use SPVM 'TestCase::Lib::SPVM::DoubleList';

# Start objects count
my $start_memory_blocks_count = SPVM::memory_blocks_count();

# SPVM::DoubleList
{
  ok(TestCase::Lib::SPVM::DoubleList->push);
  ok(TestCase::Lib::SPVM::DoubleList->pop);
  ok(TestCase::Lib::SPVM::DoubleList->newa);
  ok(TestCase::Lib::SPVM::DoubleList->unshift);
  ok(TestCase::Lib::SPVM::DoubleList->shift);
  ok(TestCase::Lib::SPVM::DoubleList->length);
  ok(TestCase::Lib::SPVM::DoubleList->get);
  ok(TestCase::Lib::SPVM::DoubleList->set);
  ok(TestCase::Lib::SPVM::DoubleList->to_array);
}

# All object is freed
my $end_memory_blocks_count = SPVM::memory_blocks_count();
is($end_memory_blocks_count, $start_memory_blocks_count);