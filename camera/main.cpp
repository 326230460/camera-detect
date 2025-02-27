#include<iostream>
#include"opencv2/opencv.hpp"
#include"face.h"



using namespace std;
using namespace cv;
using namespace aip;


int main(){
	VideoCapture cap(0);//打开默认摄像头，0表示摄像头编号，当同时有多个摄像头时，会制定编号0
	if(!cap.isOpened())
	{
		cout<<"camera open failed!"<<endl;
		return -1;//检查是否成功打开摄像头
	}
	cout<<"camera open success!"<<endl;

	Mat frame;//存放图像的容器,该容器名叫frame
	Mat grayframe;
	Mat equalizeframe;//转化后的图像


	CascadeClassifier classifier("/usr/share/opencv/haarcascades/haarcascade_frontalface_alt2.xml ");//调用opencv中的ai人脸识别的功能,需要指定到功能路径，让代码可以获取到该功能





	if (!classifier.load("/usr/share/opencv/haarcascades/haarcascade_frontalface_alt2.xml")) {//判断功能是否被成功调用
    	cout<< "Error loading face cascade" << endl;
    	return -1;
	}


	std::string app_id = "117755890";
	std::string api_key = "kdy2CLDoNiwPlw4LhBZrJZhq";
	std::string secret_key = "Cd2CbteH3am3ZfyvzltWzdhw9Pjeqq1x";
	aip::Face client(app_id, api_key, secret_key);



	string base64Imag;
	Json::Value result;

	vector<Rect> allFace;
	Mat faceImag;
	vector<uchar> jpgBuf;
	

	time_t sec;

	for(int i=0;i<20;i++){
		cap>>frame;//从摄像头中获取照片(拍照)并存放到frame中
		cvtColor(frame,grayframe,CV_BGR2GRAY);//将图像转变为灰色
		equalizeHist(grayframe,equalizeframe);//增加图像亮度和对比度
		classifier.detectMultiScale(equalizeframe,allFace);//从equalizeframe中检测到的所有人脸，全部存放到allFace中,并会依次为所有人脸以数组形式打上编号
		//classifier.detectMultiScale(frame,allFace);
		if(allFace.size()){
			rectangle(equalizeframe,allFace[0],Scalar(255,255,255));//提取第一张人脸照片作为检测,Scalar表示方框所显示的颜色
			//rectangle(frame,allFace[0],Scalar(255,255,255));
			faceImag=equalizeframe(allFace[0]);//将矩形内的人脸图片赋值给faceImag
			imencode(".jpg",faceImag,jpgBuf);//将获取到的头像转换成jpg格式.jpgBuf相当于图片的各种信息，包括内存大小，名称等

			base64Imag=base64_encode((char *)jpgBuf.data(),jpgBuf.size());
			result=client.search(base64Imag,"BASE64","Teacher",aip::null);
			//g++ main.cpp  -o main -lopencv_highgui -lopencv_core -lopencv_imgproc -lopencv_objdetect -std=c++11 -lcurl -lcrypto -ljsoncpp.......终端命令

			if(!result["result"].isNull()){

				if(result["result"]["user_list"][0]["score"].asInt()>80){
					cout<<"人脸对比度为："<<result["result"]["user_list"][0]["score"]<<"\t";
					cout<<"人脸名字为："<<result["result"]["user_list"][0]["user_id"]<<endl;
					sec=time(NULL);
					
					cout<<"时间为："<<ctime(&sec)<<endl;
					putText(equalizeframe,result["result"]["user_list"][0]["user_id"].asString(),Point(10,20),FONT_HERSHEY_SIMPLEX,0.6,Scalar(255,255,255));
					//将识别到人脸对应的人名打印在窗口中，point(0,0)表示初始位置左上角，0.6表示文字显示大小倍数，Scalar表示文字显示颜色
					putText(equalizeframe,ctime(&sec),Point(10,70),FONT_HERSHEY_SIMPLEX,0.6,Scalar(255,255,255));
				}else{
					cout<<"对比度过低，该人脸跟人脸库信息有差异"<<endl;
				}


			}else{
			cout<<"未检测到人脸"<<endl;			
			}

			//cout<<result<<endl;
			//result里会有“是否检测到人脸errror_msg”以及“识别对应人脸的准确度socre”，这两个对人脸对比具有重要影响
			//result内容如下（仅取其中一个作为参考）
			//{
				//"cached" : 0,
				//"error_code" : 0,
				//"error_msg" : "SUCCESS",
				//"log_id" : 1888010183,
				//"result" : 
				//{
					//"face_token" : "a142a130036fc50e5a773088a16e3253",
					//"user_list" : 
					//[
						//{
							//"group_id" : "Teacher",
							//"score" : 91.103294372558594,
							//"user_id" : "xiaomo",
							//"user_info" : ""
						//}
						//]
				//},
				//"timestamp" : 1740651823
			//}
			//{
				//"error_code" : 18,
				//"error_msg" : "Open api qps request limit reached"
			//}
			//{
				//"error_code" : 18,
				//"error_msg" : "Open api qps request limit reached"
			//}

		}

		imshow("video",equalizeframe);//打开video窗口，来显示最终图像
		//imshow("video",frame);
		waitKey(40);//为了能让人眼看清frame图像，需要使用该代码阻止后面程序进行，单位为毫秒
		//终端调用时，需要用到的库有：opencv_highgui以及opencv_core
		if(!result){
		cout<<"没有获取到头像任何信息"<<endl;
		return -1;
		}
	}

	if(allFace.size()){
	cout<<"检测到了人脸"<<endl;
	}else{
	cout<<"检测不到人脸"<<endl;
	}

//终端命令： ./main >> log.txt   表示将代码运行内容传递保存到log.txt文件中
return 0;
}
