#!/usr/bin/perl

use Test::More tests => 22;

sub d_exe {
  my $user = shift;
  my $name = shift;
  my $cmd = shift;

  system("docker exec -u $user $name /bin/bash -c '$cmd'");
}

sub d_run {
  my $name = shift;
  my $img = shift;
  my $cmd = shift;
  my $user = shift || qx(id -un);
  chomp($user);

  my $uid = shift || qx(id -u);
  chomp($uid);

  my $group = qx(id -gn);
  chomp($group);

  my $gid = qx(id -g);
  chomp($gid);

  system("docker rm -f $name");
  system("docker run --privileged -d -v /tmp:/tmp -v /home/$user:/home/$user " .
         "-h $name --name $name $img init");
  d_exe("root", $name, "groupadd -g $gid $group && " .
                       "useradd -M -s /bin/bash -g $gid -u $uid $user");
  d_exe($user, $name, "cd $ENV{PWD} && $cmd");
  system("docker rm -f $name");
}

my @docker_names = qx(dockerfiles/docker.sh list);

for my $doc (@docker_names) {
  chomp($doc);

  my($first, $second) = split(":", $doc, 2);
  $second =~ s/centos/el/;

  my $cmd;
  if ($second =~ m/el/ || $second =~ m/opensuse/) {
    $cmd = "rpm -i";
  }
  else {
    $cmd = "dpkg -i";
  }

  $cmd = "ls pkgs/*$second* | xargs $cmd";

  my $name = $doc;
  $name =~ s#curtine/##;
  $name =~ s/:/-/;

  d_run($name, $doc, $cmd);
}

