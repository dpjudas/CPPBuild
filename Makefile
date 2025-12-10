
CC=gcc
CX=g++

CFLAGS=-I . -I CPPBuild -D MINIZ_NO_STDIO
LIBS=
ODIR=build/obj

_OBJ=\
	CPPBuild/BuildSetup.o \
	CPPBuild/CPPBuild.o \
	CPPBuild/CSSTokenizer.o \
	CPPBuild/Main.o \
	CPPBuild/Precomp.o \
	CPPBuild/VSGenerator.o \
	CPPBuild/WebTarget.o \
	CPPBuild/Process.o \
	CPPBuild/Base64/Base64.o \
	CPPBuild/Guid/Guid.o \
	CPPBuild/IOData/DataBuffer.o \
	CPPBuild/IOData/Directory.o \
	CPPBuild/IOData/File.o \
	CPPBuild/IOData/FilePath.o \
	CPPBuild/IOData/MemoryDevice.o \
	CPPBuild/Javascript/quickjs-amalgam.o \
	CPPBuild/Javascript/ScriptContext.o \
	CPPBuild/Json/JsonValue.o \
	CPPBuild/Text/Text.o \
	CPPBuild/Text/UTF16.o \
	CPPBuild/Text/UTF8Reader.o \
	CPPBuild/Msi/MSIGenerator.o \
	CPPBuild/Zip/miniz.o \
	CPPBuild/Zip/ZipReader.o \
	CPPBuild/Zip/ZipWriter.o

OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

all: build/cppbuild

install: build/cppbuild
	cp build/cppbuild /usr/local/bin

build/cppbuild: build/createdir $(OBJ)
	@echo Linking $@
	@$(CX) -o $@ $^ $(CFLAGS) $(LIBS)

$(ODIR)/%.o: %.c build/createdir
	@echo Compiling $@
	@$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.cpp build/createdir
	@echo Compiling $@
	@$(CX) -c -o $@ $< $(CFLAGS)

build/createdir:
	@echo Creating build directory
	@mkdir build
	@mkdir build/obj
	@mkdir build/obj/CPPBuild
	@mkdir build/obj/CPPBuild/Base64
	@mkdir build/obj/CPPBuild/Guid
	@mkdir build/obj/CPPBuild/IOData
	@mkdir build/obj/CPPBuild/Javascript
	@mkdir build/obj/CPPBuild/Json
	@mkdir build/obj/CPPBuild/Text
	@mkdir build/obj/CPPBuild/Msi
	@mkdir build/obj/CPPBuild/Zip
	@touch build/createdir

clean:
	rm -r build
