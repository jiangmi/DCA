#!/bin/bash
# Author: Giovanni Balduzzi
#
# Sources the easybuild setup script specifing the project eb directory 
# and afterwards loads the DCA++ module created with easybuild.
#
# USAGE:
# source load_EB_DCA++.bash <eb load script> <eb build folder>
#     compile DCA in your build directory by using:
# cmake -C ${DCA_SOURCE}/build-aux/Cray_EB_(CPU/GPU).cmake ${DCA_SOURCE}
#
# NOTE: You might want to modify the toolchain in this script
#       and the cuda-toolkit  minor version in DCA++-CMake-CrayGNU-2015.11.eb

#default inputs
############################################################################################################
DEFAULT_EB_SCRIPT=/apps/common/easybuild/setup.sh
DEFAULT_EB_FOLDER=/project/s299/easybuild/daint #default for daint #TODO remove group default before release
TOOLCHAIN="" #leave empty for default CrayGnu,2015.11
############################################################################################################


#if the arguments are provided loads eb into from $1 and build into $2
#else revert to default.
if [ "$#" -eq 3 ]; then
EB_SCRIPT=$1
EB_FOLDER=$2
else 
echo "Using default folders"
EB_SCRIPT=$DEFAULT_EB_SCRIPT
EB_FOLDER=$DEFAULT_EB_FOLDER
fi
echo "Loading easybild from $EB_SCRIPT into $EB_FOLDER"
source $EB_SCRIPT  $EB_FOLDER

export DCA_SOURCE="$(cd "$( dirname "${BASH_SOURCE[0]}" )"/../ && pwd )"

echo "running scripts in ${DCA_SOURCE}/easybuild"
export EASYBUILD_ROBOT_PATHS=$DCA_SOURCE/easybuild:$EASYBUILD_ROBOT_PATHS
if [ $TOOLCHAIN="" ]; then
    echo "Using default toolchain"
    eb $DCA_SOURCE/easybuild/DCA++-CMake-CrayGNU-2015.11.eb -r
else
    echo "Using $TOOLCHAIN toolchain"
    eb $DCA_SOURCE/easybuild/DCA++-CMake-CrayGNU-2015.11.eb -r --try-toolchain $TOOLCHAIN
fi
echo ""
echo "Loading DCA++ ..."
module load DCA++/1.0-CrayGNU-2015.11
