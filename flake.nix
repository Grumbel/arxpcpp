{
  description = "Archive Explorer for C++";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-22.05";
    flake-utils.url = "github:numtide/flake-utils";

    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "github:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.flake-utils.follows = "flake-utils";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    uitest.url = "github:grumbel/uitest";
    uitest.inputs.nixpkgs.follows = "nixpkgs";
    uitest.inputs.flake-utils.follows = "flake-utils";
    uitest.inputs.tinycmmc.follows = "tinycmmc";

    exspcpp.url = "github:grumbel/exspcpp";
    exspcpp.inputs.nixpkgs.follows = "nixpkgs";
    exspcpp.inputs.flake-utils.follows = "flake-utils";
    exspcpp.inputs.tinycmmc.follows = "tinycmmc";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich, uitest, exspcpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in rec {
        packages = flake-utils.lib.flattenTree {
          arxpcpp = pkgs.stdenv.mkDerivation {
            pname = "arxpcpp";
            version = "0.0.0";
            src = nixpkgs.lib.cleanSource ./.;
            nativeBuildInputs = [
              pkgs.cmake
              pkgs.pkg-config
            ];
            buildInputs = [
              tinycmmc.defaultPackage.${system}
              uitest.defaultPackage.${system}
            ];
            propagatedBuildInputs = [
              logmich.defaultPackage.${system}
              exspcpp.defaultPackage.${system}

              pkgs.libarchive
              pkgs.gtest
            ];
           };
        };
        defaultPackage = packages.arxpcpp;
      });
}
