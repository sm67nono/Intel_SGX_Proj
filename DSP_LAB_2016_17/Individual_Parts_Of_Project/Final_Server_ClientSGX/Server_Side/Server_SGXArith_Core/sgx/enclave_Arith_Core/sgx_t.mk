######## SGX SDK Settings ########
SGX_SDK ?= /home/neo/IntelSGXDowloads/sgxsdk
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

Arith_Core_Cpp_Files := trusted/Arith_Core.cpp 
Arith_Core_C_Files := 
Arith_Core_Include_Paths := -IInclude -Itrusted -I$(SGX_SDK)/include -I$(SGX_SDK)/include/tlibc -I$(SGX_SDK)/include/stlport


Flags_Just_For_C := -Wno-implicit-function-declaration -std=c11
Common_C_Cpp_Flags := $(SGX_COMMON_CFLAGS) -nostdinc -fvisibility=hidden -fpie -fstack-protector $(Arith_Core_Include_Paths) -fno-builtin-printf -I.
Arith_Core_C_Flags := $(Flags_Just_For_C) $(Common_C_Cpp_Flags)
Arith_Core_Cpp_Flags :=  $(Common_C_Cpp_Flags) -std=c++11 -nostdinc++ -fno-builtin-printf -I.

Arith_Core_Cpp_Flags := $(Arith_Core_Cpp_Flags)  -fno-builtin-printf

Arith_Core_Link_Flags := $(SGX_COMMON_CFLAGS) -Wl,--no-undefined -nostdlib -nodefaultlibs -nostartfiles -L$(SGX_LIBRARY_PATH) \
	-Wl,--whole-archive -l$(Trts_Library_Name) -Wl,--no-whole-archive \
	-Wl,--start-group -lsgx_tstdc -lsgx_tstdcxx -l$(Crypto_Library_Name) -l$(Service_Library_Name) -Wl,--end-group \
	-Wl,-Bstatic -Wl,-Bsymbolic -Wl,--no-undefined \
	-Wl,-pie,-eenclave_entry -Wl,--export-dynamic  \
	-Wl,--defsym,__ImageBase=0 \
	-Wl,--version-script=trusted/Arith_Core.lds

Arith_Core_Cpp_Objects := $(Arith_Core_Cpp_Files:.cpp=.o)
Arith_Core_C_Objects := $(Arith_Core_C_Files:.c=.o)

ifeq ($(SGX_MODE), HW)
ifneq ($(SGX_DEBUG), 1)
ifneq ($(SGX_PRERELEASE), 1)
Build_Mode = HW_RELEASE
endif
endif
endif


.PHONY: all run

ifeq ($(Build_Mode), HW_RELEASE)
all: Arith_Core.so
	@echo "Build enclave Arith_Core.so  [$(Build_Mode)|$(SGX_ARCH)] success!"
	@echo
	@echo "*********************************************************************************************************************************************************"
	@echo "PLEASE NOTE: In this mode, please sign the Arith_Core.so first using Two Step Sign mechanism before you run the app to launch and access the enclave."
	@echo "*********************************************************************************************************************************************************"
	@echo 


else
all: Arith_Core.signed.so
endif

run: all
ifneq ($(Build_Mode), HW_RELEASE)
	@$(CURDIR)/app
	@echo "RUN  =>  app [$(SGX_MODE)|$(SGX_ARCH), OK]"
endif


######## Arith_Core Objects ########

trusted/Arith_Core_t.c: $(SGX_EDGER8R) ./trusted/Arith_Core.edl
	@cd ./trusted && $(SGX_EDGER8R) --trusted ../trusted/Arith_Core.edl --search-path ../trusted --search-path $(SGX_SDK)/include
	@echo "GEN  =>  $@"

trusted/Arith_Core_t.o: ./trusted/Arith_Core_t.c
	@$(CC) $(Arith_Core_C_Flags) -c $< -o $@
	@echo "CC   <=  $<"

trusted/%.o: trusted/%.cpp
	@$(CXX) $(Arith_Core_Cpp_Flags) -c $< -o $@
	@echo "CXX  <=  $<"

trusted/%.o: trusted/%.c
	@$(CC) $(Arith_Core_C_Flags) -c $< -o $@
	@echo "CC  <=  $<"

Arith_Core.so: trusted/Arith_Core_t.o $(Arith_Core_Cpp_Objects) $(Arith_Core_C_Objects)
	@$(CXX) $^ -o $@ $(Arith_Core_Link_Flags)
	@echo "LINK =>  $@"

Arith_Core.signed.so: Arith_Core.so
	@$(SGX_ENCLAVE_SIGNER) sign -key trusted/Arith_Core_private.pem -enclave Arith_Core.so -out $@ -config trusted/Arith_Core.config.xml
	@echo "SIGN =>  $@"
clean:
	@rm -f Arith_Core.* trusted/Arith_Core_t.* $(Arith_Core_Cpp_Objects) $(Arith_Core_C_Objects)
