# shell.nix

let
  pkgs = import <nixpkgs> {};

in
pkgs.mkShell {
  buildInputs = [
    pkgs.cmake
    pkgs.git
    pkgs.expat
    pkgs.python311Packages.jinja2
    pkgs.python311Packages.pyyaml
    pkgs.mbedtls
    pkgs.platformio
    pkgs.vim
  ];
}
