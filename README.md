# MTCNN-kcf
用mtcnn检测人脸，使用kcf进行人脸跟踪

使用方法，将mtcnn-kcf 复制到ncnn的examples目录下
修改该目录下的cmakelist，在最后添加：

add_executable(mtcnn-kcf ./mtcnn-kcf/main.cpp ./mtcnn-kcf/mtcnn.cpp ./mtcnn-kcf/mtcnn.h ./mtcnn-kcf/kcftracker.hpp)
target_link_libraries(mtcnn-kcf ncnn "/home/xhs/software/ncnn-master/examples/mtcnn-kcf/libKCFlib.a" ${OpenCV_LIBS})


kcf代码来自：https://github.com/joaofaro/KCFcpp
mtcnn 来自：https://github.com/moli232777144/mtcnn_ncnn
