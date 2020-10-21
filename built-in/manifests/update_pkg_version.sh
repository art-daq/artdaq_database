#!/bin/bash

new_package_versions=(
   "TRACE:v3_16_00"
#   "cetbuildtools:v7_17_01"
#   "cetpkgsupport:v1_14_01"
#   "cmake:v3_17_3"
#   "boost:v1_69_0"
#   "swig:v4_0_2"
#	 "mongodb:v4_0_8c"
)

function update_package_version() {
[[ $# -ne 3 ]] && echo "Error: Not enough arguments; args: \"$@\"" &&  return 1

 local file_name=$1
 local pkg_name=$2
 local pkg_version=$3

 local old_line=$(cat $file_name | grep $pkg_name)
 local old_pkg_version=$(echo $old_line | tr -s ' ' |cut -d " " -f2)

 local old_pkg_version_dotted=$(echo $old_pkg_version |tr '_' '.' |tr -d 'v')
 local pkg_version_dotted=$(echo $pkg_version |tr '_' '.' |tr -d 'v')
 local new_line=$(echo $old_line |  sed "s/-$old_pkg_version_dotted/-$pkg_version_dotted/g; s/$old_pkg_version/$pkg_version/g")
 rm -f $file_name.new
 sed  "/$pkg_name /c$new_line" $file_name | column -t > $file_name.new
 mv -f $file_name.new $file_name
 return 0
}

function loop_over_manifests() {
 for f in $(find -name "*MANIFEST.txt"); do update_package_version $f "$@" ;done
}

function main() {
 for new_package_version in ${new_package_versions[*]}; do
  loop_over_manifests ${new_package_version%:*} ${new_package_version#*:};
 done
}

main
