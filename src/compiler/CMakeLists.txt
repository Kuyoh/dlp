
include_directories ("${PROJECT_SOURCE_DIR}/parser/include")
include_directories ("${PROJECT_SOURCE_DIR}/jit/include")
include_directories ("${PROJECT_SOURCE_DIR}/compiler/include")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(LLVM_LIBS support core interpreter irreader target object transformutils executionengine mcjit orcjit runtimedyld native nativecodegen x86asmparser x86asmprinter x86codegen x86desc x86disassembler x86info x86utils)
# arch64 aarch64asmparser aarch64asmprinter aarch64codegen aarch64desc aarch64disassembler aarch64info
# aarch64utils all all-targets amdgpu amdgpuasmparser amdgpuasmprinter amdgpucodegen amdgpudesc amdgpudisassembler
# amdgpuinfo amdgpuutils analysis arm armasmparser armasmprinter armcodegen armdesc armdisassembler arminfo
# asmparser asmprinter binaryformat bitreader bitwriter bpf bpfasmprinter bpfcodegen bpfdesc bpfdisassembler
# bpfinfo codegen core coroutines coverage debuginfocodeview debuginfodwarf debuginfomsf debuginfopdb demangle
# dlltooldriver engine executionengine globalisel hexagon hexagonasmparser hexagoncodegen hexagondesc
# hexagondisassembler hexagoninfo instcombine instrumentation interpreter ipo irreader lanai lanaiasmparser
# lanaiasmprinter lanaicodegen lanaidesc lanaidisassembler lanaiinfo libdriver lineeditor linker lto
# mc mcdisassembler mcjit mcparser mips mipsasmparser mipsasmprinter mipscodegen mipsdesc mipsdisassembler
# mipsinfo mirparser msp430 msp430asmprinter msp430codegen msp430desc msp430info native nativecodegen nvptx
# nvptxasmprinter nvptxcodegen nvptxdesc nvptxinfo objcarcopts object objectyaml option orcjit passes powerpc
# powerpcasmparser powerpcasmprinter powerpccodegen powerpcdesc powerpcdisassembler powerpcinfo profiledata
# runtimedyld scalaropts selectiondag sparc sparcasmparser sparcasmprinter sparccodegen sparcdesc sparcdisassembler
# sparcinfo support symbolize systemz systemzasmparser systemzasmprinter systemzcodegen systemzdesc
# systemzdisassembler systemzinfo tablegen target transformutils vectorize x86 x86asmparser x86asmprinter
# x86codegen x86desc x86disassembler x86info x86utils xcore xcoreasmprinter xcorecodegen xcoredesc xcoredisassembler xcoreinfo

file(GLOB compiler_SOURCE_FILES src/*.cpp)
file(GLOB compiler_HEADER_FILES include/*.hpp)
add_executable(dlpc ${compiler_SOURCE_FILES} ${compiler_HEADER_FILES})

#link_directories(${LLVM_LIBRARY_DIRS})
target_link_libraries (dlpc dlpparser dlpjit dlprt ${LLVM_LIBS} ${DYNLOAD_LIBRARY})

add_custom_command(TARGET dlpc POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:dlprt> $<TARGET_FILE_DIR:dlpc>)
