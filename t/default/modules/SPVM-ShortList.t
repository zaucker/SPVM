use lib "t/lib";
use TestAuto;

use strict;
use warnings;

use Test::More 'no_plan';

use SPVM 'TestCase::Lib::SPVM::ShortList';

# Start objects count
my $start_memory_blocks_count = SPVM::memory_blocks_count();

# SPVM::ShortList
{
  ok(TestCase::Lib::SPVM::ShortList->push);
  ok(TestCase::Lib::SPVM::ShortList->pop);
  ok(TestCase::Lib::SPVM::ShortList->newa);
  ok(TestCase::Lib::SPVM::ShortList->unshift);
  ok(TestCase::Lib::SPVM::ShortList->shift);
  ok(TestCase::Lib::SPVM::ShortList->length);
  ok(TestCase::Lib::SPVM::ShortList->get);
  ok(TestCase::Lib::SPVM::ShortList->set);
  ok(TestCase::Lib::SPVM::ShortList->to_array);
}

# All object is freed
my $end_memory_blocks_count = SPVM::memory_blocks_count();
is($end_memory_blocks_count, $start_memory_blocks_count);