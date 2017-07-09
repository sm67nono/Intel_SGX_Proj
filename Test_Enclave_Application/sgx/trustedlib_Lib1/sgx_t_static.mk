######## SGX SDK Settings ########
SGX_SDK ?= /opt/intel/sgxsdk/
SGX_MODE ?= SIM
SGX_ARCH ?= x64

ifeq ($(shell getconf LONG_BIT), 32)
	SGX_ARCH := x86
else ifeq ($(findstring -m32, $(CXXFLAGS)), -m32)
	SGX_ARCH := x86
endif

ifeq ($(SGX_ARCH), x86)
	SGX_COMMON_CFLAGS := -m32
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x86/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x86/sgx_edger8r
else
	SGX_COMMON_CFLAGS := -m64
	SGX_LIBRARY_PATH := $(SGX_SDK)/lib64
	SGX_ENCLAVE_SIGNER := $(SGX_SDK)/bin/x64/sgx_sign
	SGX_EDGER8R := $(SGX_SDK)/bin/x64/sgx_edger8r
endif

ifeq ($(SGX_DEBUG), 1)
ifeq ($(SGX_PRERELEASE), 1)
$(error Cannot set SGX_DEBUG and SGX_PRERELEASE at the same time!!)
endif
endif

ifeq ($(SGX_DEBUG), 1)
        SGX_COMMON_CFLAGS += -O0 -g
else
        SGX_COMMON_CFLAGS += -O2
endif

ifneq ($(SGX_MODE), HW)
	Trts_Library_Name := sgx_trts_sim
	Service_Library_Name := sgx_tservice_sim
else
	Trts_Library_Name := sgx_trts
	Service_Library_Name := sgx_tservice
endif

Crypto_Library_Name := sgx_tcrypto

Lib1_Cpp_Files := static_trusted/Lib1.cpp 
Lib1_C_Files := 
Lib1_Include_Paths := -IInclude -ILib1 -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/stlport

Flags_Just_For_C := -Wno-implicit-function-declaration -std=c11
Common_C_Cpp_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector $(Lib1_Include_Paths) -fno-builtin-printf -I.
Lib1_C_Flags := $(Flags_Just_For_C) $(Common_C_Cpp_Flags)
Lib1_Cpp_Flags := $(Common_C_Cpp_Flags) -std=c++11 -nostdinc++ -fno-builtin-printf  -I.

Lib1_Cpp_Flags := $(Lib1_Cpp_Flags)  -fno-builtin-printf

Lib1_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tstdcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 \
	-Wl,--version-script=trusted/Lib1.lds


Lib1_Cpp_Objects := $(Lib1_Cpp_Files:.cpp=.o)
Lib1_C_Objects := $(Lib1_C_Files:.c=.o)

ifeq ($(SGX_MODE), HW)
ifneq ($(SGX_DEBUG), 1)
ifneq ($(SGX_PRERELEASE), 1)
Build_Mode = HW_RELEASE
endif
endif
endif


.PHONY: all run

all: libLib1.sgx.static.lib.a

######## Lib1 Objects ########

static_trusted/Lib1_t.h: $(SGX_EDGER8R) ./static_trusted/Lib1.edl
	@cd ./static_trusted && $(SGX_EDGER8R) --header-only  --trusted ../static_trusted/Lib1.edl --search-path ../static_trusted --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

static_trusted/Lib1_t.o: ./trusted/Lib1_t.c
	@$(CC) $(Lib1_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

static_trusted/%.o: static_trusted/%.cpp
	@$(CXX) $(Lib1_Include_Paths) $(Lib1_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

static_trusted/%.o: static_trusted/%.c
	@$(CC) $(Lib1_C_Flags) -c $< -o $@
	@echo "CC  <=  $<"

libLib1.sgx.static.lib.a: static_trusted/Lib1_t.h $(Lib1_Cpp_Objects) $(Lib1_C_Objects)
	ar rcs libLib1.sgx.static.lib.a $(Lib1_Cpp_Objects) $(Lib1_C_Objects)  
	@echo "LINK =>  $@"

clean:
	@rm -f Lib1.* static_trusted/Lib1_t.* $(Lib1_Cpp_Objects) $(Lib1_C_Objects)
