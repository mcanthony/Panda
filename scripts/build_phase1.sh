#!/bin/bash

# "phase 1" of the build is performed using the bootstrap compiler, and 
# "phase 2" is performed using first the generated Java compiler and then the 
# generated native compiler. Being able to run these two phases separately can
# make incompatible changes easier.

export BASEDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )/.."
cd "$BASEDIR"
source scripts/setup-script.sh
export PANDA_HOME="$BASEDIR/bootstrap"
export PANDAC="java -ea -Xmx2g -jar $BASEDIR/bootstrap/java/pandac/bin/pandac.jar -L $NATIVE_TARGET/core/lib $@"

export DYLD_LIBRARY_PATH="\"$NATIVE_TARGET/core/lib\":\"$PANDA_HOME/native/core/lib\""
PANDAC_SRC=src/pandac/panda/org/pandalanguage/pandac
CORE_FILES="src/core/panda/panda/core/*.panda\
        src/core/panda/panda/collections/*.panda src/core/panda/panda/threads/*.panda\
        src/core/panda/panda/io/*.panda src/core/panda/panda/math/*.panda"

echo "Compiling core (Java)..."

mkdir -p $JAVA_TARGET/core/java

$PANDAC -XnoCoreLib -f javah -o $JAVA_TARGET/core/java $CORE_FILES
mkdir -p $JAVA_TARGET/core/lib
$PANDAC -XnoCoreLib -f java -o $JAVA_TARGET/core/java $CORE_FILES
mkdir -p $JAVA_TARGET/core/classes
cp -r $BASEDIR/src/core/java/* $JAVA_TARGET/core/java
cd $JAVA_TARGET/core/java
javac -Xmaxerrs 10000 -J-Xmx512m -source 1.8 -d $JAVA_TARGET/core/classes `find . -name "*.java"`
cd $BASEDIR
mkdir -p $SHARED_TARGET/core/lib
$PANDAC -XnoCoreLib -f plink -o $SHARED_TARGET/core/lib/PandaCoreClasses.plink $CORE_FILES
jar cf $JAVA_TARGET/core/lib/PandaCoreClasses.jar  -C $JAVA_TARGET/core/classes .

mkdir -p $JAVA_TARGET/pandac/bin
cp $JAVA_TARGET/core/lib/PandaCoreClasses.jar $JAVA_TARGET/pandac/bin/panda.jar
export PANDA_HOME="$BASEDIR/build"

echo "Creating parser..."
$PANDAC -o $JAVA_TARGET/parsergenerator.jar -f jar src/pandac/parser/ParserGenerator.panda src/pandac/parser/GrammarParser.panda src/pandac/parser/Action.panda src/pandac/parser/Parser.panda src/pandac/parser/ParserState.panda src/pandac/parser/Reducer.panda src/pandac/parser/StateNode.panda
mkdir -p $SHARED_TARGET
java -jar $JAVA_TARGET/parsergenerator.jar src/pandac/parser/panda.grammar $SHARED_TARGET/PandaLRParser.panda

echo "Compiling pandac (Java)..."

STATIC_SETTINGS=$SHARED_TARGET/StaticSettings.panda
echo "package org.pandalanguage.pandac.compiler" > "$STATIC_SETTINGS"
echo "class StaticSettings {" >> "$STATIC_SETTINGS"
echo "constant TARGET_TRIPLE := " >> "$STATIC_SETTINGS"
if [ `uname` = "Darwin" ]; then
    echo "'x86_64-apple-macosx10.8.0'" >> "$STATIC_SETTINGS"
else
    echo "'x86_64-pc-linux-gnu'" >> "$STATIC_SETTINGS"
fi
echo "constant GCC_EXE_ARGS:ImmutableArray<String> := [" >> "$STATIC_SETTINGS"
if [ `uname` != "Darwin" ]; then
    echo "'-lunwind'" >> "$STATIC_SETTINGS"
fi
echo "]" >> "$STATIC_SETTINGS"
echo "constant GCC_LIBRARY_ARGS:ImmutableArray<String> := [" >> "$STATIC_SETTINGS"
echo "]" >> "$STATIC_SETTINGS"
echo "constant LLC_EXE_ARGS := ['-O3'" >> "$STATIC_SETTINGS"
if [ `uname` = "Darwin" ]; then
    echo ", '-mattr=-avx'" >> "$STATIC_SETTINGS"
fi
echo "]" >> "$STATIC_SETTINGS"
echo "constant LLC_LIBRARY_ARGS:ImmutableArray<String> := ['-O3'" >> "$STATIC_SETTINGS"
if [ `uname` = "Darwin" ]; then
    echo ", '-mattr=-avx'" >> "$STATIC_SETTINGS"
else
    echo ", '-relocation-model=pic'" >> "$STATIC_SETTINGS"
fi
echo "]" >> "$STATIC_SETTINGS"
echo "constant PANDAUI_GCC_ARGS:ImmutableArray<String> := [" >> "$STATIC_SETTINGS"
if [ `uname` = "Darwin" ]; then
    echo "'-l', 'pandaui', '-l', 'SDL2', '-l', 'SDL2_image', " >> "$STATIC_SETTINGS"
    echo "'-L/opt/local/lib', '-lcairo', " >> "$STATIC_SETTINGS"
    echo "'-L', '/opt/local/lib', '-L', '\$PANDA_HOME/native/ui/lib/'" >> "$STATIC_SETTINGS"
else
    echo "'-lpandaui', '-lSDL2', '-lSDL2_image', '-L/usr/lib/SDL2', '-L/usr/lib/x86_64-linux-gnu/', " >> "$STATIC_SETTINGS"
    echo "'-L', '/opt/local/lib', '-L', '\$PANDA_HOME/native/ui/lib/'" >> "$STATIC_SETTINGS"
fi
echo "]" >> "$STATIC_SETTINGS"
echo "}" >> "$STATIC_SETTINGS"

mkdir -p "$JAVA_TARGET/pandac/bin"
$PANDAC -XpreserveTempArtifacts -o "$JAVA_TARGET/pandac/bin/pandac.jar" -f jar `find src/pandac/panda -name "*.panda"` \
    "$STATIC_SETTINGS" $SHARED_TARGET/PandaLRParser.panda src/pandac/parser/Action.panda src/pandac/parser/Parser.panda \
    src/pandac/parser/ParserState.panda src/pandac/parser/Reducer.panda src/pandac/parser/StateNode.panda
