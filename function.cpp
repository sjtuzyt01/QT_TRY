//------------------------------------
// Created by Linghan 2017-01-03
//------------------------------------

#include "function.h"
#include <opencv2\highgui\highgui.hpp>
#include <iostream>
#include <windows.h>
#include "stdio.h"
#include <QApplication>
#include <QtWidgets/QtWidgets>
using namespace cv;

int count;
int scale = 40;
int niches = 7; // upper(256/40)=7
bool drawFlag = false;
bool selectFlag = true;
Rect rect;
int totPlayers = 0;
int startp = 0;

vector<Point2f> srcpt(4);
vector<Point2f> dstpt(4);

PlayerRec players[1001][25];

int position[540][32] = { 0 };
double pu[23][200001];
double stdpu[23][200001];
double lastpu[200001];
double w[100][100];
int TrackingStatus[25]={2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2};

//-------------Item Type Constant Definition----------------------
int FOOTBALL=0;
int TEAM_1_GOALKEEPER=1;
int TEAM_1_PLAYER=2;
int TEAM_2_GOALKEEPER=3;
int TEAM_2_PLAYER=4;
//------------Tracking Status Constant Definition-----------------
int REGULAR=0;
int OUT_OF_WINDOW=1;
int NOT_INCLUDED=2;
//----------------------------------------------------------------
/**
  @brief 根据路径得到文件
  @param[in] string path: 文件路径
  @param[in] string exd: 文件扩展名
  @param[in] vector<string>& files: 所有文件名集合
*/
void getFiles(string path, string exd, vector<string>& files){
    //文件句柄
    long   hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string pathName, exdName;

    if (strcmp(exd.c_str(), "") != 0){
        exdName = "\\*." + exd;
    }else{
        exdName = "\\*";
    }

    if ((hFile = _findfirst(pathName.assign(path).append(exdName).c_str(), &fileinfo)) != -1){
        do{
            //如果是文件夹中仍有文件夹,迭代之
            //如果不是,加入列表
            if ((fileinfo.attrib &  _A_SUBDIR)){
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles(pathName.assign(path).append("\\").append(fileinfo.name), exd, files);
            }else{
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    files.push_back(pathName.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }
}

//----------------------------------------------------------------
/**
  @brief 根据名称分析视频来自视角1还是视角2
*/
int getFileAngle(string filename){
    int len_of_file_name;
    int len_of_find_spcial_sign;
    len_of_file_name = filename.length();
    len_of_find_spcial_sign = filename.find('-');
    if (filename[len_of_find_spcial_sign - 1] - '0' == 1){
        return 1;
    }else{
        return 2;
    }
}

//----------------------------------------------------------------
/**
 @brief 选择transform Mat的src图上四个点
*/
void pickPointSrc(int event, int x, int y, int type, void *param) {

    int mxwidth =  QApplication::desktop()->width();
    int mxheight = QApplication::desktop()->height();
    double imgScale = (mxwidth*mxheight)*0.8/(1920*1080);
    if (event == EVENT_LBUTTONDOWN && count<4) {
        srcpt[count] = Point2f(x/imgScale, y/imgScale);
        printf("srcpt[%d]:(%.1f, %.1f)\n", count, srcpt[count].x, srcpt[count].y);
        count++;
    }

    if (event == EVENT_LBUTTONDOWN && count == 4)
        destroyWindow("videoView");
}

//----------------------------------------------------------------
/**
 @brief 选择transform Mat的des图上四个点
*/
void pickPointDst(int event, int x, int y, int type, void *param) {

    if (event == EVENT_LBUTTONDOWN && count<4) {
        dstpt[count] = Point2f(x, y);
        printf("dstpt[%d]:(%.1f, %.1f)\n", count, dstpt[count].x, dstpt[count].y);
        count++;
        //std::cout << dstpt[count].x<< dstpt[count].y << std::endl;
    }

    if (event == EVENT_LBUTTONDOWN && count == 4)
        destroyWindow("birdView");
}

//----------------------------------------------------------------
//----- 1.mp4 -----
/*if (tag == 1){
    srcpt[0] = Point2f(-198, 213);
    srcpt[1] = Point2f(353, 119);
    srcpt[2] = Point2f(988, 159);
    srcpt[3] = Point2f(270, 552);

    dstpt[0] = Point2f(8, 22);
    dstpt[1] = Point2f(303, 22);
    dstpt[2] = Point2f(303, 478);
    dstpt[3] = Point2f(8, 478);
}

//----- 2.mp4 -----
if (tag == 2){
    srcpt[0] = Point2f(462, 270);
    srcpt[1] = Point2f(849, 117);
    srcpt[2] = Point2f(592, 85);
    srcpt[3] = Point2f(134, 169);

    dstpt[0] = Point2f(243, 406);
    dstpt[1] = Point2f(243, 95);
    dstpt[2] = Point2f(68, 95);
    dstpt[3] = Point2f(68, 406);
}*/

/**
 @brief 得到transform Mat
*/
Mat getTransformMat(Mat &src, Mat &dst, int Video) {

    int mxwidth =  QApplication::desktop()->width();
    int mxheight = QApplication::desktop()->height();


//    double imgScale = (mxwidth*mxheight)*0.8/(1920*1080);
//    Size dsize = Size(src.cols*imgScale, src.rows*imgScale); // shrink image by 60%
//    Mat tempimg1 = Mat(dsize, CV_32S);
//    resize(src, tempimg1, dsize);

//    count = 0;
//    cv::namedWindow("videoView");
//    imshow("videoView",tempimg1);
//    cv::setMouseCallback("videoView", pickPointSrc);
//    waitKey(0);

//    count = 0;
//    cv::namedWindow("birdView");
//    imshow("birdView", dst);
//    cv::setMouseCallback("birdView", pickPointDst);
//    waitKey(0);


    srcpt[0] = Point2f(112, 414);
    srcpt[1] = Point2f(988, 1056);
    srcpt[2] = Point2f(2096, 576);
    srcpt[3] = Point2f(1376, 404);

    dstpt[0] = Point2f(10, 480);
    dstpt[1] = Point2f(300, 478);
    dstpt[2] = Point2f(304, 23);
    dstpt[3] = Point2f(10, 23);


    Mat transMat = getPerspectiveTransform(srcpt, dstpt);

    vector<Point2f> points, points_trans;
    for (int i = 0; i<src.rows; i++){
        for (int j = 0; j<src.cols; j++) {
            points.push_back(Point2f(j, i));
        }
    }
    perspectiveTransform(points, points_trans, transMat);
    Mat img_trans = Mat::zeros(dst.rows, dst.cols, CV_8UC3);
    int count = 0;
    for (int i = 0; i<src.rows; i++){
        uchar* p = src.ptr<uchar>(i);
        for (int j = 0; j<src.cols; j++){
            int y = points_trans[count].y;
            int x = points_trans[count].x;
            //printf("%d, %d, y:%d, 3x:%d\n",dst.rows, dst.cols, y, 3*x);
            if (y >= 0 && y<dst.rows && x >= 0 && x<dst.cols) {
                uchar* t = img_trans.ptr<uchar>(y);
                t[x * 3] = p[j * 3];
                t[x * 3 + 1] = p[j * 3 + 1];
                t[x * 3 + 2] = p[j * 3 + 2];
                //printf("%u,%u,%u\n",t[3*x],t[3*x+1],t[3*x+2]);
            }
            count++;
        }
    }

    //imshow("afterTransformation", img_trans);
    imwrite("F:/Study/TrackingPro/TrackQt/TrackQt/result/afterTransform.png",img_trans);
    return transMat;
}

//-------------------------------------------------------------
/**
 @brief 记录矩形
*/
void recordRect(int x1, int y1, int x2, int y2, int type) {
    //rectangle(frame, cvPoint(x1,y1), cvPoint(x2,y2), cvScalar(0, 0, 255, 0));
    players[0][totPlayers] = PlayerRec(y1, x1, y2, x2, type); // notice! swap x and y
    TrackingStatus[totPlayers]=REGULAR;
    totPlayers++;
}

//----------------------------------------------------------------
/**
 @brief 鼠标选定跟踪框位置
*/
void on_mouse(int event, int x, int y, int type, void *param) {
    int mxwidth =  QApplication::desktop()->width();
    int mxheight = QApplication::desktop()->height();
    double imgScale = (mxwidth*mxheight)*0.8/(1920*1080);

    int& otype = *(int*)param;
    switch (event) {
    case(EVENT_LBUTTONDOWN) : {
                                  drawFlag = true;
                                  rect = Rect(x, y, 0, 0);
                                  printf("from (x:%d, y:%d) ", x, y);
                                  break;
    }
    case(EVENT_MOUSEMOVE) : {
                                if (drawFlag)
                                {
                                    rect.width = x - rect.x;
                                    rect.height = y - rect.y;
                                }
                                break;
    }
    case(EVENT_LBUTTONUP) : {
                                drawFlag = false;
                                printf("to (x:%d, y:%d) type:%d\n", x, y, otype);
                                recordRect(rect.tl().x/imgScale, rect.tl().y/imgScale,
                                           rect.br().x/imgScale, rect.br().y/imgScale, otype);
                                break;
    }
    case(EVENT_RBUTTONDOWN) : { // 右键退出选择
                                  selectFlag = false;
                                  printf("finish selection!\n");
                                  break;
    }
    }
}

//----------------------------------------------------------------
/**
 @brief 初始化, 手工标定球、守门员、1队队员、2队队员位置
*/
void initialization(Mat &frame) {

    Mat img;
    Mat tempimg;
    int type;

    int mxwidth =  QApplication::desktop()->width();
    int mxheight = QApplication::desktop()->height();
//    QDesktopWidget* desktopWidget = QApplication::desktop();
//        QRect screenRect = desktopWidget->screenGeometry();
//        g_nActScreenW = screenRect.width();
//        g_nActScreenH = screenRect.height();


    frame.copyTo(img);



    double imgScale = (mxwidth*mxheight)*0.8/(1920*1080);
    Size dsize = Size(img.cols*imgScale, img.rows*imgScale); // shrink image by 60%
    Mat tempimg1 = Mat(dsize, CV_32S);
    resize(img, tempimg1, dsize);

    totPlayers = 0;

    selectFlag = true;
    type = FOOTBALL;
    printf("Please select the position of the ball\n");
    namedWindow("Ball");
    setMouseCallback("Ball", on_mouse, (void*)&type);
    while (selectFlag){
        tempimg1.copyTo(tempimg);
        if (drawFlag)
            rectangle(tempimg, rect.tl(), rect.br(), cvScalar(0, 255, 255, 0));

        imshow("Ball", tempimg);
        if (waitKey(10) == 27) break; //keyvalue27:esc
        //if (totPlayers>=1)break;//1 ball
    }
    if(totPlayers<1)totPlayers=1;
    //std::cout<<players[0][0].x1<<" "<<players[0][0].x2<<" "<<players[0][0].type<<std::endl;
    destroyWindow("Ball");

    selectFlag = true;
    type = TEAM_1_GOALKEEPER;
    printf("Please select the positions of team 1 goal keeper\n");
    namedWindow("Goal Keeper Team 1");
    setMouseCallback("Goal Keeper Team 1", on_mouse, (void*)&type);
    while (selectFlag) {
        tempimg1.copyTo(tempimg);
        if (drawFlag)
            rectangle(tempimg, rect.tl(), rect.br(), cvScalar(0, 0, 0, 0));

        imshow("Goal Keeper Team 1", tempimg);
        if (waitKey(10) == 27) break;
        //if (totPlayers>=2)break;//1 goal keeper
    }
    if(totPlayers<2)totPlayers=2;
    destroyWindow("Goal Keeper Team 1");

    selectFlag = true;
    type = TEAM_1_PLAYER;
    printf("Please select the positions of team 1 players\n");
    namedWindow("Team 1");
    setMouseCallback("Team 1", on_mouse, (void*)&type);
    while (selectFlag){
        tempimg1.copyTo(tempimg);
        if (drawFlag)
            rectangle(tempimg, rect.tl(), rect.br(), cvScalar(0, 0, 255, 0));

        imshow("Team 1", tempimg);
        if (waitKey(10) == 27) break;
        //if(totPlayers>=12)break;//10 normal players
    }
    if(totPlayers<12)totPlayers=12;
    destroyWindow("Team 1");

    selectFlag = true;
    type = TEAM_2_GOALKEEPER;
    printf("Please select the positions of team 2 goal keeper\n");
    namedWindow("Goal Keeper Team 2");
    setMouseCallback("Goal Keeper Team 2", on_mouse, (void*)&type);
    while (selectFlag){
        tempimg1.copyTo(tempimg);
        if (drawFlag)
            rectangle(tempimg, rect.tl(), rect.br(), cvScalar(255, 0, 0, 0));

        imshow("Goal Keeper Team 2", tempimg);
        if (waitKey(10) == 27) break;
        //if(totPlayers>=13)break;//1 goal keeper
    }
    if(totPlayers<13)totPlayers=13;
    destroyWindow("Goal Keeper Team 2");

    selectFlag = true;
    type = TEAM_2_PLAYER;
    printf("Please select the positions of team 2 players\n");
    namedWindow("Team 2");
    setMouseCallback("Team 2", on_mouse, (void*)&type);
    while (selectFlag){
        tempimg1.copyTo(tempimg);
        if (drawFlag)
            rectangle(tempimg, rect.tl(), rect.br(), cvScalar(255, 0, 0, 0));

        imshow("Team 2", tempimg);
        if (waitKey(10) == 27) break;
        //if(totPlayers>=23)break;//10 normal players
    }
    //if(totPlayers<22)
    totPlayers=23;

    //RISKY!
    destroyWindow("Team 2");
    //imshow("src", frame);
    printf("total targets: %d\n", totPlayers);
    //for(int i=0;i<23;i++)std::cout<<TrackingStatus[i]<<' ';
    //std::cout<<std::endl;
    //for(int i=0;i<23;i++)std::cout<<players[0][i].type<<' ';
    //std::cout<<std::endl;

}

//-------------------------------------------------------------

double kernelProfile(int x, int y, int centerx, int centery, double h) {
    double dist = (centerx - x)*(centerx - x) + (centery - y)*(centery - y);
    dist = dist / (h*h);
    //printf("%lf\n", exp(-0.5*dist));
    return exp(-0.5*dist);
}

//-------------------------------------------------------------

double gvalue(int x, int y, int centerx, int centery, double h) {
    //printf("enter\n");
    double dist = (centerx - x)*(centerx - x) + (centery - y)*(centery - y);
    dist = dist / (h*h);
    //printf("return\n");
    return -0.5 * exp(-0.5*dist);
}

//-------------------------------------------------------------

void showPlan(int imgIndex, Mat transMat, Mat &field) {
    vector<Point2f> pts, pts_trans;
    pts.clear();

    for (int p = startp; p < totPlayers; p++) {
        double footy = (players[imgIndex][p].y1 + players[imgIndex][p].y1) / 2;
        double footx = players[imgIndex][p].x2;
        pts.push_back(Point2f(footy, footx));
    }

    perspectiveTransform(pts, pts_trans, transMat);

    for (int p = startp; p < totPlayers; p++) {
        Point2f tpt = pts_trans[p];
        int type = players[0][p].type;
        CvScalar color;
        if (type == 1) color = Scalar(0, 0, 200); // red
        else if (type == 2) color = Scalar(0, 200, 200, 0); // yellow
        else if (type == 0) color = Scalar(0, 0, 0, 0); // black
        else color = Scalar(200, 0, 0, 0); // blue

        if(pts_trans[p].x >0 && pts_trans[p].x <312){
            players[imgIndex][p].x_trans = pts_trans[p].x /312.0 * 500.0;
        }
        else{
            players[imgIndex][p].x_trans = 0;
        }
        if(pts_trans[p].y >0 && pts_trans[p].y <500){
            players[imgIndex][p].y_trans = pts_trans[p].y / 500.0 * 800.0;
            std::cout << players[imgIndex][p].y_trans;
        }
        else{
            players[imgIndex][p].y_trans = 0;
        }

        circle(field, tpt, 5, color, -1);
    }
    imshow("field", field);

}

//-------------------------------------------------------------

void Correct_mouse(int event, int x, int y, int ind, void *param) {
    int mxwidth =  QApplication::desktop()->width();
    int mxheight = QApplication::desktop()->height();
    double imgScale = (mxwidth*mxheight)*0.8/(1920*1080);

    //2 parameters passed, protection needed
    int* parameter=(int*)param;
    int index = parameter[0];
    int playerIndex=parameter[1];
    switch (event) {
    case(EVENT_LBUTTONDOWN) : {
             drawFlag = true;
             rect = Rect(x, y, 0, 0);
             printf("New Position -- from (x:%d, y:%d) ", x, y);
             break;
        }
    case(EVENT_MOUSEMOVE) : {
             if (drawFlag)
             {
                 rect.width = x - rect.x;
                 rect.height = y - rect.y;
             }
             break;
        }
    case(EVENT_LBUTTONUP) : {
             drawFlag = false;
             printf("to (x:%d, y:%d)\n", x, y);
             //Save type of player, change place
             int playerType=players[index][playerIndex].type;
             players[index][playerIndex] = PlayerRec(rect.tl().y/imgScale, rect.tl().x/imgScale, rect.br().y/imgScale, rect.br().x/imgScale, playerType);
             selectFlag = false;
             break;
        }
    }
}

//-------------------------------------------------------------

void reSelect(Mat &frame, int imgIndex, int &x, int &y, int playerIndex){
    Mat img;
    Mat tempimg;

    int mxwidth =  QApplication::desktop()->width();
    int mxheight = QApplication::desktop()->height();
//    QDesktopWidget* desktopWidget = QApplication::desktop();
//        QRect screenRect = desktopWidget->screenGeometry();
//        g_nActScreenW = screenRect.width();
//        g_nActScreenH = screenRect.height();


    frame.copyTo(img);



    double imgScale = (mxwidth*mxheight)*0.8/(1920*1080);
    Size dsize = Size(img.cols*imgScale, img.rows*imgScale); // shrink image by 60%
    Mat tempimg1 = Mat(dsize, CV_32S);
    resize(img, tempimg1, dsize);


    drawFlag = false;
    selectFlag = true;
    namedWindow("reSelect");
    int MouseParam[2]={};
    MouseParam[0]=imgIndex;
    MouseParam[1]=playerIndex;
    setMouseCallback("reSelect", Correct_mouse, (void*)MouseParam);
//    Mat tempimg;
    while(selectFlag){
        tempimg1.copyTo(tempimg);
        cvtColor(tempimg,tempimg,CV_RGB2BGR);
        if(drawFlag)
            rectangle(tempimg, rect.tl(), rect.br(), cvScalar(0, 255, 255, 0));

        imshow("reSelect",tempimg);
        if(waitKey(10) == 27) break; //keyvalue27:esc
    }
    destroyWindow("reSelect");
    x = players[imgIndex][playerIndex].x2;
    y = players[imgIndex][playerIndex].y2;
    TrackingStatus[playerIndex]=REGULAR;
}


