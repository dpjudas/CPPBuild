
CC=gcc
CX=g++

CFLAGS=-I . -I CPPBuild -D MINIZ_NO_STDIO --std=gnu11
CXXFLAGS=-I . -I CPPBuild -D MINIZ_NO_STDIO --std=c++20
LIBS=-lssl -lcrypto
ODIR=build/obj

_OBJ=\
	CPPBuild/BuildSetup.o \
	CPPBuild/CPPBuild.o \
	CPPBuild/CSSTokenizer.o \
	CPPBuild/Main.o \
	CPPBuild/Precomp.o \
	CPPBuild/VSGenerator.o \
	CPPBuild/Target.o \
	CPPBuild/ConsoleProcess.o \
	CPPBuild/VSWorkspace.o \
	CPPBuild/MakefileWorkspace.o \
	CPPBuild/Package.o \
	CPPBuild/PackageManager.o \
	CPPBuild/FileTimeCache.o \
	CPPBuild/Base64/Base64.o \
	CPPBuild/Guid/Guid.o \
	CPPBuild/ProcessMutex.o \
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
	CPPBuild/Zip/ZipWriter.o \
	CPPBuild/HttpClient/HttpClient.o \
	CPPBuild/HttpClient/HttpConnection.o \
	CPPBuild/HttpClient/HttpConnectionImpl.o \
	CPPBuild/HttpClient/SocketAddress.o \
	CPPBuild/HttpClient/SocketStream.o \
	CPPBuild/HttpClient/TcpSocket.o \
	CPPBuild/HttpClient/TlsStream.o \
	CPPBuild/HttpClient/TlsStreamOpenSSL.o

OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

all: build/cppbuild

install: build/cppbuild
	cp build/cppbuild /usr/local/bin

build/cppbuild: build/createdir $(OBJ)
	@echo Linking $@
	@$(CX) -o $@ $^ $(CXXFLAGS) $(LIBS)

$(ODIR)/%.o: %.c build/createdir
	@echo Compiling $@
	@$(CC) -c -o $@ $< $(CFLAGS)

$(ODIR)/%.o: %.cpp build/createdir
	@echo Compiling $@
	@$(CX) -c -o $@ $< $(CXXFLAGS)

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
	@mkdir build/obj/CPPBuild/HttpClient
	@touch build/createdir

clean:
	rm -r build
