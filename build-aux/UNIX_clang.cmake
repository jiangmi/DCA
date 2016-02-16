################################################################################
# Initial cache list for UNIX 
# Usage: cmake -C path/to/UNIX_clang.cmake ..
################################################################################

set(EBROOTNFFT "/usr/local" CACHE FILEPATH "NFFT location (no /lib)")
set(EBROOTSPGLIB "/usr/local" CACHE FILEPATH "idem as before")
set(EBROOTGTEST "${CMAKE_SOURCE_DIR}/libs/gmock-1.7.0/gtest/" CACHE FILEPATH "folder with gtest")
# Compilers for MPI
set(MPI_CXX_COMPILER /usr/bin/mpic++)
set(MPI_C_COMPILER /usr/bin/mpicc)

option(DCA_GPU_SUPPORT "Disable GPU support." OFF)


# MPIEXEC stuff for executing parallel tests.
# Note: CXX and CC must be set as environment variables to the corresponding MPI
#       C++ and C compiler wrappers. //Giovanni: not really a good practice
set(MPIEXEC "/usr/bin/mpirun"
  CACHE FILEPATH "Executable for running MPI programs.")
set(MPIEXEC_NUMPROC_FLAG "-np"
  CACHE FILEPATH
  "Flag used by MPI to specify the number of processes for MPIEXEC; the next option will be the number of processes.")
