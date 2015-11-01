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

# compile and run
t_system_ok "Taste t/test.tas t/test.in > t/test.out";
t_system_ok "diff -w t/test.exp t/test.out";

# error messages with different EOL chars
for my $eol ("\n", "\r", "\r\n") {
	path("test1.tas")->spew_raw($eol,$eol,"program Test.",$eol);
	t_system_err "Taste test1.tas t/test.in 2> t/test.out";
	is path("t/test.out")->slurp, "-- line 3 col 13: \"{\" expected\n";
}

unlink "test1.tas", "t/test.out";
done_testing;


