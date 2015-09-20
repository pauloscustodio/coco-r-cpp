#!/user/bin/perl

use strict;
use warnings;
use Test::More;
use Path::Tiny;

sub t_system($$) {
	my($cmd, $ret) = @_;
	ok 1, $cmd;
	is !!$ret, !!system($cmd);
}
sub t_system_ok($)  { t_system($_[0], 0); }
sub t_system_err($) { t_system($_[0], 1); }

t_system_ok "Taste t/test.tas t/test.in > t/test.out";
t_system_ok "diff -w t/test.exp t/test.out";

done_testing;


