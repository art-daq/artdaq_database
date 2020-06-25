#/bin/bash
[[ -z ${1+x} ]] || cd $1

ycm_file=.ycm_extra_conf.py

cat <<EOF1 > ${ycm_file}
def FlagsForFile(filename, **kwargs):
        return {
                'flags': ['-x', 'c++', '-Wall', '-Wextra', '-Werror',
              '-std=c++17',
              '-nostdinc++',

     "-I/daq/software/products/clang/v7_0_0/Linux64bit+3.10-2.17/include/c++/v1",

EOF1

find .  -type f -name "*.h*" -print  |cut -d"/" -f2 |sort -u |xargs -I {} ls -d $(pwd)/{} |awk '{printf("     \"-I%s\",\n",$1);}' >> ${ycm_file}
echo >> ${ycm_file}

printenv |grep _INC= |grep -v localProducts |cut -d"=" -f2|awk '{printf("     \"-I%s\",\n",$1);}' >> ${ycm_file}

cat <<EOF2 >> ${ycm_file}
             ],
}
EOF2
