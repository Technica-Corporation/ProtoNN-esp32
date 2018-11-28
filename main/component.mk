#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the
# src/ directory, compile them and link them into lib(subdirectory_name).a
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

CXXFLAGS += -ggdb -Og -DSINGLE -DL2 -D_GLIBCXX_USE_C99 -DSPARSE_LABEL_PROTONN \
-DSPARSE_LABEL_BONSAI -DEIGEN_USE_BLAS -DLOGGER=1 -DVERBOSE=1
