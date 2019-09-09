#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

/*
    Autores
    Lindelmo
    Thiago

    Trabalho da disciplina de Computação Visual
*/

int main() {

    //Inicializa a câmera de vídeo padrão.
    VideoCapture camera(0);
    camera.set(CV_CAP_PROP_FRAME_WIDTH,1024);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT,600);
    camera.set(CV_CAP_PROP_FPS, 25);

    //Se a câmera não funcionar, então termina o programa com falha.
    if(!camera.isOpened()) {
        return EXIT_FAILURE;
    }

    Mat frame;
    Mat primeira;
    Mat segunda;
    Mat hsv;
    Mat mao;

    //Kernel utilizado para as operações morfológicas.
    Mat kernel;
    kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

    cv::Rect rect(10, 10, 450, 500);

    int maiorA = 0;
    int indexA = 0;

    int valor = 21;
    namedWindow( "window_name", CV_WINDOW_AUTOSIZE );
    createTrackbar("Depth0", "window_name", &valor, 255);


    namedWindow("Camera", CV_WINDOW_AUTOSIZE );
    namedWindow("Mao1", CV_WINDOW_AUTOSIZE );

    //Início do laço de captura da câmera.
    while (true) {
        camera >> frame;
        if (!frame.empty()) {
            char c = (char) waitKey(1);
            if (c == 27) {
                break;
            }
            cv::flip(frame,frame,1);
            cv::cvtColor(frame,hsv,COLOR_BGR2HSV);
            mao = hsv(rect);

            //SEGMENTANDO A IMAGEM EM 2 RANGES PARA A COR DA PELE
            cv::inRange(mao,Scalar(0,30,0),Scalar(20,255,255),primeira);
            cv::inRange(mao,Scalar(170,30,0),Scalar(180,255,255),segunda);
            mao = primeira|segunda;

            cv::dilate(mao,mao,kernel,Point(-1,-1),2);
            cv::erode(mao,mao,kernel,Point(-1,-1),2);
            cv::GaussianBlur(mao,mao,Size(7,7),0);

            vector<vector<Point> > contours;
            vector<Vec4i> hierarchy;
            cv::findContours(mao,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE);
            maiorA = 0;
            indexA = 0;
            for (int i = 0; i < contours.size(); i++){
                double a = cv::contourArea(contours[i],false);
                if (a > maiorA){
                    maiorA = a;
                    indexA = i;
                }
            }

            std::vector<std::vector<int> > hull(1);
            std::vector<std::vector<cv::Vec4i> >convDef(1);

            cv::convexHull(cv::Mat(contours[indexA]),hull[0],false,true);
            cv::convexityDefects(contours[indexA],hull[0],convDef[0]);
            int contador = 0;

            for (int i = 0; i < convDef[0].size(); i++){
                float depth = convDef[0][i][3]/256.0;
                if (depth > valor){
                    int id = convDef[0][i][0];
                    contours[indexA][id].x += 10;
                    contours[indexA][id].y += 10;
                    cv::circle(frame,contours[indexA][id],5,cv::Scalar(0,0,255),-1);
                    contador++;
                }
            }

            cv::rectangle(frame, rect, cv::Scalar(0, 255, 0),2);

            //Colocando informações textuais na tela
            putText(frame,"JOGADOR 1 : " + std::to_string(contador), Point2f(150,550), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
            putText(frame,"JOGADOR 1 : " + std::to_string(contador), Point2f(150,550), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);
            imshow("Camera",frame);
            imshow("Mao1",mao);
        }
    }
    //Libera o recurso da câmera.
    camera.release();
    //Fecha todas as janelas.
    destroyAllWindows();

    return EXIT_SUCCESS;
}


