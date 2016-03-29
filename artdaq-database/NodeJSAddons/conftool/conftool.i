%module conftool

%{
    #include "conftool.h"
%}

%include "std_pair.i"
%include "std_string.i"
%template(result_pair_t) std::pair<bool,std::string>;

%include "conftool.h"