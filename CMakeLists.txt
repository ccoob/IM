cmake_minimum_required(VERSION 3.5.0)
project(GateServer VERSION 0.1.0 LANGUAGES C CXX)

#set(CMAKE_CXX_STANDARD 17)

if(MSVC)
    add_compile_options("/source-charset:utf-8")
    add_compile_options(-D_WIN32_WINNT=0x0A00) #添加编译选项
endif()

#添加预处理定义
#add_compile_definitions(_WIN32_WINNT=0x0A00)


#添加cmake库配置文件的查找目录
set(CMAKE_PREFIX_PATH D:/WorkSpace/boost_1_82_0/stage/lib/cmake; D:/WorkSpace/grpc/install/lib/cmake;)

#添加库组件
find_package(Boost CONFIG COMPONENTS json url filesystem)

find_package(Protobuf CONFIG REQUIRED)  #未知原因，只写gRPC会找不到protobuf，只好显示指明了
find_package(gRPC CONFIG REQUIRED)

get_filename_component(msg_proto "message.proto" ABSOLUTE)      #proto文件的全路径（绝对路径+文件名）
get_filename_component(msg_proto_path ${msg_proto} PATH)        #proto文件的路径
set(msg_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/message.pb.cc") #要生成的pb文件
set(msg_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/message.pb.h")
set(msg_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.cc")
set(msg_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/message.grpc.pb.h")

#通过protoc.exe生成pb文件
add_custom_command(             
      OUTPUT "${msg_proto_srcs}" "${msg_proto_hdrs}" "${msg_grpc_srcs}" "${msg_grpc_hdrs}"
      COMMAND protoc
      --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
      --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
      -I "${msg_proto_path}"
      --plugin=protoc-gen-grpc="D:\\WorkSpace\\grpc\\install\\bin\\grpc_cpp_plugin.exe"
      "${msg_proto}"
      DEPENDS "${msg_proto}"
)

#message("${CMAKE_CURRENT_BINARY_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}") #将pb文件所在的目录添加进include

#将生成的四个pb文件合成一个库
add_library(msg_grpc_proto   
  ${msg_grpc_srcs}
  ${msg_grpc_hdrs}
  ${msg_proto_srcs}
  ${msg_proto_hdrs}
)

#将pb文件生成的库再链接上其它需要的库
target_link_libraries(msg_grpc_proto
  #absl::check
  gRPC::grpc++_reflection
  gRPC::grpc++
  protobuf::libprotobuf
)

set(HEADERS 
    Cserver.h const.h HttpConnection.h singleton.h LogicSystem.h VerifygRPCClient.h "ConfigMgr.h" "AsioIOServicePool.h" )

set(SOURCES 
    main.cpp CServer.cpp HttpConnection.cpp LogicSystem.cpp ConfigMgr.cpp "AsioIOServicePool.cpp" "VerifygRPCClient.cpp")

add_executable(GateServer ${HEADERS} ${SOURCES} config.ini)


#增加头文件包含目录
target_include_directories(GateServer PRIVATE "D:\\WorkSpace\\boost_1_82_0")

#链接项目所需的所有库
target_link_libraries(GateServer PRIVATE 
    Boost::json Boost::url Boost::filesystem
    ws2_32 mswsock 
	gRPC::grpc++ gRPC::grpc++_reflection protobuf::libprotobuf  msg_grpc_proto
	#absl::check
	#absl::flags
	#absl::flags_parse
	#absl::log
)

