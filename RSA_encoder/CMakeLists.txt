cmake_minimum_required(VERSION 3.10)
project(RSA_Project)#прост название

set(CMAKE_CXX_STANDARD 17)#стандарт с++ прост
set(CMAKE_CXX_STANDARD_REQUIRED ON)# мог бы закрыть прогу если бы C++ был бы не 17 версии

add_executable(rsa_crypto #добавляет в исполняемый файл
    main.cpp
    BigInteger.cpp
    RSA.cpp
)

target_include_directories(rsa_crypto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})#добавляет директории в которых заголовочные файлы будет искать
