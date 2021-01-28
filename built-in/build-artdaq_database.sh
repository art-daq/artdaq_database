#!/bin/bash

# build in ${working_dir}/build
# copyback directory is ${working_dir}/copyBack

usage()
{
  cat 1>&2 <<EOF
Usage: $(basename ${0}) [-h]

Options:

  -h    This help.

EOF
}

working_dir=${WORKSPACE:-${HOME}/builds}
version=${VERSION}
qual_set="${QUAL}"
build_type=${BUILDTYPE:-prof}
git_branch=${ARTDAQ_DATABASE_GITBRANCH:-develop}

squal=${qual_set%:*}
basequal=${qual_set##*:}

dotver=$(echo ${version} | sed -e 's/_/./g' | sed -e 's/^v//')

echo "building the artdaq_database distribution for ${version} ${dotver} ${qual_set} ${build_type}"

OS=$(uname)
if [ "${OS}" = "Linux" ]
then
    flvr=slf$(lsb_release -r | sed -e 's/[[:space:]]//g' | cut -f2 -d":" | cut -f1 -d".")
elif [ "${OS}" = "Darwin" ]
then
  flvr=d$(uname -r | cut -f1 -d".")
else 
  echo "ERROR: unrecognized operating system ${OS}"
  exit 1
fi
echo "build flavor is ${flvr}"
echo ""

qualdir=$(echo ${qual_set} | sed -e 's%:%-%')

set -x

srcdir=${working_dir}/source
blddir=${working_dir}/build
productsdir=${working_dir}/products


function cleanup() {
# start with clean directories
rm -rf ${blddir}
rm -rf ${srcdir}
rm -rf ${working_dir}/copyBack 
# now make the dfirectories
mkdir -p ${srcdir} || return 11
mkdir -p ${blddir} || return 12
mkdir -p ${working_dir}/copyBack || return 13
}

function checkout_source() {
  echo
  echo "checkout source"
  echo

  cd ${blddir} || return 21

  local_src=/mnt/sde/mu2etrg/lukhanin-develop001/srcs/artdaq-utilities-database
	if [[ -d ${local_src} ]]; then
	 	mkdir -p ${srcdir}/artdaq-database 
		cp -r  ${local_src}/* ${srcdir}/artdaq-database/
	else 
	  git clone http://cdcvs.fnal.gov/projects/artdaq-utilities-database ${srcdir}/artdaq-database
    cd ${srcdir}/artdaq-database
    git checkout ${git_branch}
	fi
}

function pull_products() {
  if [ ! -d ${productsdir} ]; then
    mkdir -p ${productsdir} || return 31
  fi

  cd ${productsdir} || return 

  if [ ! -d ${srcdir}/artdaq-database/built-in/manifests/ ]; then
    echo "Directory ${srcdir}/artdaq-database/built-in/manifests is missing"
    return 32
  fi
   
  cp ${srcdir}/artdaq-database/built-in/manifests/artdaq_database-build-*MANIFEST.txt  ${productsdir}/

  curl --fail --silent --location --insecure -O http://scisoft.fnal.gov/scisoft/bundles/tools/pullProducts || exit 1
  curl --fail --silent --location --insecure -O http://scisoft.fnal.gov/scisoft/bundles/tools/pullPackage || exit 1
  chmod +x ${productsdir}/pull*

  # we pull what we can so we don't have to build everything
  ${productsdir}/pullProducts -l ${productsdir} ${flvr} artdaq_database-build $(echo  ${squal}| sed -e 's/:/-/g')-${basequal} ${build_type}

  ${productsdir}/pullPackage ${productsdir} sl7  git-v2_20_1

  # Remove any artdaq_database that came with the bundle
  if [ -d ${productsdir}/artdaq_database ]; then
    echo "Removing ${productsdir}/artdaq_database"
    rm -rf ${productsdir}/artdaq_database
  fi

  set +x

  source ${productsdir}/setup
  set -x
}


function run_build() {
  echo
  echo "begin build"
  echo

  cd ${blddir} || return 41
  
  if [[ "${build_type}" == "prof" ]]; then
    build_flag="-p"
  else
    build_flag="-d"
  fi
  set +x

  prodblddir=${blddir}/build-artdaq_database-$(echo ${qual_set} | sed -e 's/://g')${build_type}

  mkdir -p  ${prodblddir}  || return 42
  cd ${prodblddir}

  source ${srcdir}/artdaq-database/ups/setup_for_development ${build_flag} ${basequal} $(echo  ${squal} | sed -e 's/:/ /g')
  setup git 

  ups active
  export MAKE_FHICLCPP_STATIC=TRUE
  export MAKE_CETLIBEXCEPT_STATIC=TRUE
  export MAKE_CETLIB_STATIC=TRUE


  unset RUN_TESTS
  CETPKG_J=$(nproc)
  buildtool -p -j$CETPKG_J 2>&1 |tee ${blddir}/build_artdaq-database.log || \
  { mv ${blddir}/*.log  ${working_dir}/copyBack/
    return 43 
  }

  export RUN_TESTS=true
  buildtool -j$CETPKG_J 2>&1 |tee ${blddir}/build_tests_artdaq-database.log || \
  { mv ${blddir}/*.log  ${working_dir}/copyBack/
    return 44 
  }

  CETPKG_J=1
   
 	export TRACE_NAME=$(echo ${qual_set} | sed -e 's/://g')${build_type}
  export TRACE_FILE=/tmp/trace_buffer_$TRACE_NAME
	[[ -f $TRACE_FILE ]] &&  rm $TRACE_FILE

  export TRACE_MSGMAX=2000
  export TRACE_NUMENTS=100000
  tinfo
  tenv
  treset

  export PATH=${prodblddir}/bin:$PATH
  export LD_LIBRARY_PATH=${prodblddir}/lib:$LD_LIBRARY_PATH

  buildtool -t -j$CETPKG_J 2>&1 |tee ${blddir}/test_artdaq-database.log || \
  { mv ${blddir}/*.log  ${working_dir}/copyBack/
    mv ${prodblddir}/{test,Testing}  ${working_dir}/copyBack/
		tshow > ${working_dir}/copyBack/trace-$TRACE_NAME.txt 
    return 45 
  }

	tshow > ${working_dir}/copyBack/trace-$TRACE_NAME.txt 
}

function stash_artifacts() {
  echo
  echo "move files"
  echo

  mv ${prodblddir}/*.bz2  ${working_dir}/copyBack/
  mv ${blddir}/*.bz2  ${working_dir}/copyBack/
  mv ${blddir}/*.txt  ${working_dir}/copyBack/
  mv ${blddir}/*.log  ${working_dir}/copyBack/
  mv ${blddir}  ${working_dir}/copyBack/
  mv ${srcdir}  ${working_dir}/copyBack/

  echo
  echo "cleanup"
  echo
  rm -rf ${blddir}
  rm -rf ${srcdir}
}

function patch_products() {
#fix mongodb
  cd ${productsdir}/mongodb/v3_4_6d
  find ./ -name  "mongodbConfig.cmake" -type f -print        | xargs -n 1 sed -i "s/3.4.6c/3.4.6d/g"
  find ./ -name  "mongodbConfig.cmake" -type f -print        | xargs -n 1 sed -i "s/v3_4_6c/v3_4_6d/g"
  find ./ -name  "mongodbConfigVersion.cmake" -type f -print | xargs -n 1 sed -i "s/3.4.6c/3.4.6d/g"
  find ./ -name  "mongodbConfigVersion.cmake" -type f -print | xargs -n 1 sed -i "s/v3_4_6c/v3_4_6d/g"
  cd ${productsdir}/mongodb/v4_0_8
  find ./ -name  "libbson-1.0-config.cmake" -type f -print   | xargs -n 1 sed -i "s|}/include/libbson-1.0|}/../include/libbson-1.0|g" 
  find ./ -name  "libmongoc-1.0-config.cmake" -type f -print | xargs -n 1 sed -i "s|}/include/libmongoc-1.0|}/../include/libmongoc-1.0|g"  
  cd ${productsdir}/mongodb/v4_0_8a
  find ./ -name  "libbson-1.0-config.cmake" -type f -print   | xargs -n 1 sed -i "s|}/include/libbson-1.0|}/../include/libbson-1.0|g" 
  find ./ -name  "libmongoc-1.0-config.cmake" -type f -print | xargs -n 1 sed -i "s|}/include/libmongoc-1.0|}/../include/libmongoc-1.0|g"  
#  find ./ -name  "mongodb.table" -type f -print              | xargs -n 1 sed -i '/clang/{n;d}'
}	

cleanup
RC=$?
if [ $RC -ne 0 ]; then
   echo "Error: Failed initial cleanup. Aborting. "; exit 1
fi

checkout_source
RC=$?
if [ $RC -ne 0 ]; then
   echo "Error: Failed checking-out artdaq_database source. Aborting. "; exit 1
fi

pull_products
RC=$?
if [ $RC -ne 0 ]; then
   echo "Error: Failed pulling build dependency products. Aborting. "; exit 1
fi

patch_products

run_build
RC=$?
if [ $RC -ne 0 ]; then
   echo "Error: Failed running artdaq_database build. Aborting. "; exit 1
fi


stash_artifacts
RC=$?
if [ $RC -ne 0 ]; then
   echo "Error: Failed stashing artdaq_database build artifacts. Aborting. "; exit 1
fi

[[ $(ls ${working_dir}/copyBack/artdaq_database*.tar.bz2 |wc -l ) -eq 1 ]] \
      ||  { echo "Error: No artdaq_database*.tar.bz2 found in the copyBack directory."; exit 1; }

exit 0

