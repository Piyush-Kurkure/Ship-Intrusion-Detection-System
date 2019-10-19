#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include<QMessageBox>
#include<QInputDialog>
#include<QLineEdit>
#include<QDebug>
#include <QProcess>


#include "./smtp_client/src/SmtpMime"


#include <opencv2/core.hpp>
#include "opencv2/opencv.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/ml.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include <ctime>
#include <string.h>
#include <ctime>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <iostream>

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"

const char* keys =
"{help h usage ? | | Usage examples: \n\t\t./object_detection_yolo.out --image=dog.jpg \n\t\t./object_detection_yolo.out --video=run_sm.mp4}"
"{image i        |<none>| input image   }"
"{video v       |<none>| input video   }"
;

using namespace cv;
using namespace cv::ml;
using namespace std;
using namespace dnn;
using namespace std;


//extra variables
int top1,baseLine1,left1,height1,width1,right1,bottom1;
Size labelSize1;

// Initialize the parameters
float confThreshold = 0.5; // Confidence threshold
float nmsThreshold = 0.4;  // Non-maximum suppression threshold
int inpWidth = 416;  // Width of network's input image
int inpHeight = 416; // Height of network's input image
vector<string> classes;

// Remove the bounding boxes with low confidence using non-maxima suppression
void postprocess(Mat& frame, const vector<Mat>& out);

// Draw the predicted bounding box
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame);

// Get the names of the output layers
vector<String> getOutputsNames(const Net& net);


static QString video_path;
std::string v_path;

bool was_previously_training = true;
QString LOGGED_IN_USER_EMAIL = "";

//EXTRA UTIL FUNCTIONS

int SELECTED_CAMERA_INDEX = -1, TESTING_CAMERA_INDEX = -1;
VideoCapture feed;


bool is_dir(const char* path) {
    struct stat buf;
    stat(path, &buf);
    return S_ISDIR(buf.st_mode);
}

int round_up(int numToRound, int multiple){
    if (multiple == 0) return numToRound;
    int remainder = numToRound % multiple;
    if (remainder == 0) return numToRound;
    return numToRound + multiple - remainder;
}

// BACKEND FUNCTIONS

// QT INTERFACE EVENT RESPONSE FUNCTIONS START HERE

QSqlDatabase db; // must do: create table user(username, password);

void MainWindow::timerEvent(QTimerEvent *event){

    ui->training_lcd->setText(QTime::currentTime().toString(" hh:mm:ss "));
    ui->testing_lcd->setText(QTime::currentTime().toString(" hh:mm:ss "));
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setFixedSize(1000, 600);
    ui->setupUi(this);
    startTimer(1000); //For clock


    ui->training_negative_video->hide();
    ui->training_negative_button->hide();
    ui->negative_video_info->hide();
    ui->training_camera_selection->hide();
    ui->negative_duration->hide();
    ui->negative_duration_value->hide();
    ui->train_button->hide();
    ui->training_progress->hide();

    ui->testing_video_button->hide();

//    QProcess process;
//    string command = "printf";
//    QString arguments = "$USER";

//    process.start(command, arguments, QIODevice::ReadOnly);
//    process.waitForFinished(-1);

//    QString stdout = process.readAllStandardOutput();
//    QString stderr = process.readAllStandardError();
//    qInfo() << stdout << stderr;

    db = QSqlDatabase::addDatabase("QSQLITE");
//    QDir directory = QFileInfo("mainwindow.cpp").absoluteDir();
//    QString current_dir = directory.currentPath();
    db.setDatabaseName(QDir::currentPath().append("/users.db"));

//    QString Dir_Name = directory.dirName();
//    current_dir.replace(Dir_Name,"");
//    QString DB_Path(current_dir);
//    DB_Path.append("user_list.db");
//    db.setDatabaseName(DB_Path);
    if(!db.open()){
           ui->db_status->setText("DB error");
           ui->db_status->setStyleSheet("color:red;font-size:16px;font-weight:bold");
    }
    else{
        ui->db_status->setText("     DB CONNECTED");
        ui->db_status->setStyleSheet("text-align:center;color:green;font-size:16px;font-weight:bold");
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_login_username_textChanged(const QString &arg1)
{
    ui->login_response->clear();
}

void MainWindow::on_login_password_textChanged(const QString &arg1)
{
    ui->login_response->clear();
}

void MainWindow::on_login_button_clicked()
{
    QString username, password;
    username = ui->login_username->text();
    password = ui->login_password->text();
    ui->login_username->clear();
    ui->login_password->clear();
    ui->login_response->clear();

    if(!db.isOpen()){
        qDebug() << "Failed to open db";
        return;
    }
    QSqlQuery query;
    query.exec("create table user(username, password)");
    if(query.exec("select * from user where username='" + username + "' and password='" + password + "'")){
        int count = 0;
        while(query.next()){
            count++;
        }
        if(count == 1){
            ui->stackedWidget->setCurrentIndex(2); // Goes to Surveillance page by default
            LOGGED_IN_USER_EMAIL = username;// Setting user email ID
//            ui->training_page_training->setStyleSheet("border: 5px solid black");
        }
        else{
            ui->stackedWidget->setCurrentIndex(0);
            ui->login_response->setText("Invalid username/password! Please retry.");
            ui->login_response->setStyleSheet("color:red;font-size:16px;");
//            QMessageBox::information(this,"Error","Invalid username/password! Please retry.", QMessageBox::Ok);
        }
    }
    else{
        QMessageBox::information(this,"Error","Database lookup failed. Please register.", QMessageBox::Ok);
        ui->stackedWidget->setCurrentIndex(0);
    }
}

void MainWindow::on_register_button_clicked()
{
    QString username, password;
    username = ui->login_username->text();
    password = ui->login_password->text();
    ui->login_username->clear();
    ui->login_password->clear();

    QSqlQuery query;
    if(username != "" || password != ""){
        if(query.exec("select * from user where username='" + username + "' and password='" + password + "'")){
            int count = 0;
            while(query.next()){
                count++;
            }
            if(count == 1){
                QMessageBox::information(this,"Error","User already exists. Try logging in.", QMessageBox::Ok);
                ui->stackedWidget->setCurrentIndex(0);
            }
            else{
                if(query.exec("insert into user (username, password) values ('" + username + "','" + password + "')")){
                    QMessageBox::StandardButton option;
                    option = QMessageBox::information(this,"Registration Success","You have successfully registered",QMessageBox::Ok);
                    if(option==QMessageBox::Ok){
                        QMessageBox::information(this, "Information", "Log in to continue", QMessageBox::Ok);
                        ui->stackedWidget->setCurrentIndex(0);
                    }
                }
                else{
                    QMessageBox::information(this, "Error", "Registration failed. Try again", QMessageBox::Ok);
                    ui->stackedWidget->setCurrentIndex(0);
                }
            }
        }
    }
    else{
        QMessageBox::information(this, "Information", "Enter username and password to register.", QMessageBox::Ok);
    }
}

void MainWindow::on_login_page_quit_clicked()
{
    this->close();
}

void MainWindow::on_training_page_quit_clicked()
{
    this->close();
}

void MainWindow::on_testing_page_quit_clicked()
{
    this->close();
}

void MainWindow::on_training_page_training_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->training_positive_directory->clear();
}

void MainWindow::on_training_page_testing_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->training_positive_directory->clear();
}


void MainWindow::on_testing_page_testing_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->testing_model_path->clear();
}

void MainWindow::on_training_positive_button_clicked()
{
    QString positive_dir_path = QFileDialog::getExistingDirectory(this, tr("Choose catalog"), "/home/", QFileDialog::ReadOnly);
    ui->training_positive_directory->setText(positive_dir_path);
}

void MainWindow::on_training_negative_button_clicked()
{
    QString negative_video_path = QFileDialog::getOpenFileName(this,tr("File Name"),"/home/","Video Files(*.mp4 *.MOV *.mkv)");
    ui->training_negative_video->setText(negative_video_path);
}

void MainWindow::on_train_button_clicked()
{
        ui->training_progress->show();
        ui->training_progress->setValue(0);

        String positive_folder = ui->training_positive_directory->text().toUtf8().constData();
        String negative_video = ui->training_negative_video->text().toUtf8().constData();
        //int negative_duration_seconds = ui->negative_duration->value();

        //QDir directory = QFileInfo("mainwindow.cpp").absoluteDir();
        //QString current_dir = directory.currentPath();
        //QString Dir_Name = directory.dirName();
        //current_dir.replace(Dir_Name,"");
       //QString DB_Path(current_dir);
        QString datetime = QDateTime::currentDateTime().toString();
        QString DB_Path = QDir::currentPath().append("/");

        bool ok;
        QString text = QInputDialog::getText(this, tr("MODEL INFO"),
                                             tr("Any relevant information about the model:"), QLineEdit::Normal,
                                             QDir::home().dirName(), &ok);
        if (ok && !text.isEmpty()){
            DB_Path.append("(");
            DB_Path.append(text);
            DB_Path.append(") ");
        }

        DB_Path.append(datetime);

        string model_path = DB_Path.toStdString();

        system(("mkdir -p '" + model_path + "'").c_str());

}

void MainWindow::on_training_page_quit_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    LOGGED_IN_USER_EMAIL = "";
}

void MainWindow::on_training_page_quit_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    LOGGED_IN_USER_EMAIL = "";
}

void MainWindow::on_testing_help_button_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    was_previously_training = false;
}

void MainWindow::on_help_back_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);

}

void MainWindow::on_testing_model_button_clicked()
{
   QString model_dir_path = QFileDialog::getOpenFileName(this,tr("File Name"),"/home/","Video Files(*.mp4 *.MOV *.mkv)");
   ui->testing_model_path->setText(model_dir_path);
   video_path = model_dir_path;
   v_path = video_path.toUtf8().constData();
}


void MainWindow::on_about_button1_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_about_back1_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_sample_clicked()
{
//    std::string terminal_command = "./object_detection_yolo.out --video="+v_path;
//    char * writable = new char[terminal_command.size() + 1];
//    std::copy(terminal_command.begin(), terminal_command.end(), writable);
//    writable[terminal_command.size()] = '\0';
//    cout<<terminal_command;

//    QProcess::execute(writable);
//    v_path = "";


    //    CommandLineParser parser(argc, argv, keys);
    //        parser.about("Use this script to run object detection using YOLO3 in OpenCV.");
    //        if (parser.has("help"))
    //        {
    //            parser.printMessage();
    //            return 0;
    //        }
            // Load names of classes
            string classesFile = "coco.names";
            ifstream ifs(classesFile.c_str());
            string line;
            while (getline(ifs, line)) classes.push_back(line);

            // Give the configuration and weight files for the model
            String modelConfiguration = "yolov3-tiny.cfg";
            String modelWeights = "yolov3-tiny.weights";

            //Write weights
            ofstream file;
            file.open ("codebind.txt");
            file << modelWeights;
            file.close();

            // Load the network
            Net net = readNetFromDarknet(modelConfiguration, modelWeights);
            net.setPreferableBackend(DNN_BACKEND_OPENCV);
            net.setPreferableTarget(DNN_TARGET_CPU);

            // Open a video file or an image file or a camera stream.
            string str, outputFile;
            VideoCapture cap;
            VideoWriter video;
            Mat frame, blob;



                outputFile = "yolo_out_cpp.avi";


                    // Open the video file
                    //str = parser.get<String>("video");
                    ifstream ifile(v_path);
                    if (!ifile) throw("error");
                    cap.open(v_path);
                    v_path.replace(v_path.end()-4, v_path.end(), "_yolo_out_cpp.avi");
                    outputFile = v_path;

                // Open the webcam
//               else{



//                }





            // Get the video writer initialized to save the output video
    //        if (!parser.has("image")) {
    //            video.open(outputFile, VideoWriter::fourcc('M','J','P','G'), 28, Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)));
    //        }

            // Create a window
            static const string kWinName = "Deep learning object detection in OpenCV";
            namedWindow(kWinName, WINDOW_NORMAL);
            resizeWindow(kWinName, 1000,600);

            // Process frames.


            while (waitKey(1) < 0)
            {
                // get frame from the video
                cap >> frame;

                // Stop the program if reached end of video
                if (frame.empty()) {
                    cout << "Done processing !!!" << endl;
                    cout << "Output file is stored as " << outputFile << endl;
                    waitKey(3000);
                    break;
                }
            int a=0;
            if(a%1==0){
                // Create a 4D blob from a frame.
                blobFromImage(frame, blob, 1/255.0, cvSize(inpWidth, inpHeight), Scalar(0,0,0), true, false);

                //Sets the input to the network
                net.setInput(blob);

                // Runs the forward pass to get output of the output layers
                vector<Mat> outs;
                net.forward(outs, getOutputsNames(net));

                // Remove the bounding boxes with low confidence
                postprocess(frame, outs);

                // Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
                vector<double> layersTimes;
                double freq = getTickFrequency()/1000;
                double t = net.getPerfProfile(layersTimes) / freq;
                string label = format("Inference time for a frame : %.2f ms", t);
                putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

                // Write the frame with the detection boxes
                Mat detectedFrame;
                frame.convertTo(detectedFrame, CV_8U);
    //~~            if (parser.has("image")) imwrite(outputFile, detectedFrame);
                    video.write(detectedFrame);
                }
            //I added this
            //rectangle(frame, Point(left1, top1 - round(1.5*labelSize1.height1)), Point(left1 + round(1.5*labelSize1.width1), top1 + baseLine1), Scalar(255, 255, 255), FILLED);
             rectangle(frame, Point(left1, top1), Point(right1, bottom1), Scalar(255, 178, 50), 3);
        /////////////////////////////////////

        ////////////////////////////////////

            a++;


                imshow(kWinName, frame);

            }

            cap.release();
    //        if (!parser.has("image")) video.release();

}

void MainWindow::on_camera_clicked()
{
    //    CommandLineParser parser(argc, argv, keys);
    //        parser.about("Use this script to run object detection using YOLO3 in OpenCV.");
    //        if (parser.has("help"))
    //        {
    //            parser.printMessage();
    //            return 0;
    //        }
            // Load names of classes
            string classesFile = "coco.names";
            ifstream ifs(classesFile.c_str());
            string line;
            while (getline(ifs, line)) classes.push_back(line);

            // Give the configuration and weight files for the model
            String modelConfiguration = "yolov3-tiny.cfg";
            String modelWeights = "yolov3-tiny.weights";

            //Write weights
            ofstream file;
            file.open ("codebind.txt");
            file << modelWeights;
            file.close();

            // Load the network
            Net net = readNetFromDarknet(modelConfiguration, modelWeights);
            net.setPreferableBackend(DNN_BACKEND_OPENCV);
            net.setPreferableTarget(DNN_TARGET_CPU);

            // Open a video file or an image file or a camera stream.
            string str, outputFile;
            VideoCapture cap;
            VideoWriter video;
            Mat frame, blob;



                outputFile = "yolo_out_cpp.avi";

    //            if (parser.has("video"))
    //            {
    //                // Open the video file
    //                str = parser.get<String>("video");
    //                ifstream ifile(str);
    //                if (!ifile) throw("error");
    //                cap.open(str);
    //                str.replace(str.end()-4, str.end(), "_yolo_out_cpp.avi");
    //                outputFile = str;
    //            }
                // Open the webcam
//               else{
            cap.open(0);
            str="123456789";
            str.replace(str.end()-4, str.end(), "_yolo_out_cpp.avi");
                outputFile = str;


//                }





            // Get the video writer initialized to save the output video
    //        if (!parser.has("image")) {
    //            video.open(outputFile, VideoWriter::fourcc('M','J','P','G'), 28, Size(cap.get(CAP_PROP_FRAME_WIDTH), cap.get(CAP_PROP_FRAME_HEIGHT)));
    //        }

            // Create a window
            static const string kWinName = "Deep learning object detection in OpenCV";
            namedWindow(kWinName, WINDOW_NORMAL);
            resizeWindow(kWinName, 1000,600);

            // Process frames.


            while (waitKey(1) < 0)
            {
                // get frame from the video
                cap >> frame;

                // Stop the program if reached end of video
                if (frame.empty()) {
                    cout << "Done processing !!!" << endl;
                    cout << "Output file is stored as " << outputFile << endl;
                    waitKey(3000);
                    break;
                }
            int a=0;
            if(a%1==0){
                // Create a 4D blob from a frame.
                blobFromImage(frame, blob, 1/255.0, cvSize(inpWidth, inpHeight), Scalar(0,0,0), true, false);

                //Sets the input to the network
                net.setInput(blob);

                // Runs the forward pass to get output of the output layers
                vector<Mat> outs;
                net.forward(outs, getOutputsNames(net));

                // Remove the bounding boxes with low confidence
                postprocess(frame, outs);

                // Put efficiency information. The function getPerfProfile returns the overall time for inference(t) and the timings for each of the layers(in layersTimes)
                vector<double> layersTimes;
                double freq = getTickFrequency()/1000;
                double t = net.getPerfProfile(layersTimes) / freq;
                string label = format("Inference time for a frame : %.2f ms", t);
                putText(frame, label, Point(0, 15), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 255));

                // Write the frame with the detection boxes
                Mat detectedFrame;
                frame.convertTo(detectedFrame, CV_8U);
    //~~            if (parser.has("image")) imwrite(outputFile, detectedFrame);
                    video.write(detectedFrame);
                }
            //I added this
            //rectangle(frame, Point(left1, top1 - round(1.5*labelSize1.height1)), Point(left1 + round(1.5*labelSize1.width1), top1 + baseLine1), Scalar(255, 255, 255), FILLED);
             rectangle(frame, Point(left1, top1), Point(right1, bottom1), Scalar(255, 178, 50), 3);
        /////////////////////////////////////

        ////////////////////////////////////

            a++;


                imshow(kWinName, frame);

            }

            cap.release();
    //        if (!parser.has("image")) video.release();
}
//aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
void MainWindow::on_test_butt_clicked()
{

}












void postprocess(Mat& frame, const vector<Mat>& outs)
{
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    int test=0,b=3;
    if(test%b==0){
        for (size_t i = 0; i < outs.size(); ++i)
        {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));
            height1=height;
            width1=width;
            }
        }
        }
    }//b++;

    // Perform non maximum suppression to eliminate redundant overlapping boxes with
    // lower confidences
    vector<int> indices;
    NMSBoxes(boxes, confidences, confThreshold, nmsThreshold, indices);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect box = boxes[idx];
        drawPred(classIds[idx], confidences[idx], box.x, box.y,
                 box.x + box.width, box.y + box.height, frame);
    }
}

// Draw the predicted bounding box
void drawPred(int classId, float conf, int left, int top, int right, int bottom, Mat& frame)
{
    //Draw a rectangle displaying the bounding box
    rectangle(frame, Point(left, top), Point(right, bottom), Scalar(255, 178, 50), 3);
    left1=left;top1=top;right1=right;bottom1=bottom;

    //Get the label for the class name and its confidence
    string label = format("%.2f", conf);
    if (!classes.empty())
    {
        CV_Assert(classId < (int)classes.size());
        label = classes[classId] + ":" + label;
    }

    //Display the label at the top of the bounding box
    int baseLine;
    Size labelSize = getTextSize(label, FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
    top = max(top, labelSize.height);
    rectangle(frame, Point(left, top - round(1.5*labelSize.height)), Point(left + round(1.5*labelSize.width), top + baseLine), Scalar(255, 255, 255), FILLED);
    //Extra var.


    //End extra var
    putText(frame, label, Point(left, top), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,0),1);
}

// Get the names of the output layers
vector<String> getOutputsNames(const Net& net)
{
    static vector<String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = net.getUnconnectedOutLayers();

        //get the names of all the layers in the network
        vector<String> layersNames = net.getLayerNames();

        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}











