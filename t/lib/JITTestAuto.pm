package JITTestAuto;

use strict;
use warnings;

use FindBin;
use lib "t/lib";
use File::Find;
use File::Basename 'basename', 'dirname';

sub import {
  if ($FindBin::Bin =~ /\/jit$/) {
    $ENV{SPVM_TEST_LIB_DIR} = "t/jit/lib";
    $ENV{SPVM_ENABLE_JIT} = 1;
    $ENV{SPVM_BUILD_DIR} = 'spvm_build';
    push @INC, $ENV{SPVM_TEST_LIB_DIR};
    my $test_jit_dir = 't/jit/lib';

    find(
      {
        wanted => sub {
          my $package_name = $File::Find::name;
          if ($package_name =~ /\.spvm$/) {
            $package_name =~ s|t/jit/lib||;
            $package_name =~ s|^/SPVM/||;
            $package_name =~ s|/|::|g;
            $package_name =~ s|\.spvm$||;
            
            push @SPVM::JIT_PACKAGE_NAMES, $package_name;
          }
        },
        no_chdir => 1,
      },
      $test_jit_dir
    );
  }
  else {
    $ENV{SPVM_TEST_LIB_DIR} = "t/default/lib";
    $ENV{SPVM_BUILD_DIR} = 'spvm_build';
    push @INC, $ENV{SPVM_TEST_LIB_DIR};
  }
}

1;

=pod

=DESCRITPION

if test scritp file is in jit directory, jit test is automatically on.
