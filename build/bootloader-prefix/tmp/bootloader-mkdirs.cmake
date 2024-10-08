# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/esp-idf-config/v5.1.4/esp-idf/components/bootloader/subproject"
  "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader"
  "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader-prefix"
  "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader-prefix/tmp"
  "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader-prefix/src/bootloader-stamp"
  "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader-prefix/src"
  "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/RafaelDias/Projetos_Acls/gateway_Teste_Lora/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
