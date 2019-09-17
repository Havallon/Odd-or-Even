#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <string>
#include <cstdlib>


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

    camera.set(CV_CAP_PROP_FRAME_WIDTH,640);
    camera.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    camera.set(CV_CAP_PROP_FPS, 15);

    //Se a câmera não funcionar, então termina o programa com falha.
    if(!camera.isOpened()) {
        return EXIT_FAILURE;
    }

    Mat frame;
    Mat primeira;
    Mat segunda;
    Mat hsv;
    Mat mao;
    int numero = 0;

    int win = 0;
    bool play = false;
    //Kernel utilizado para as operações morfológicas.
    Mat kernel;
    kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));
    //cv::Rect rect(340, 0, 300, 300);
    cv::Rect rect(0, 0, 300, 300);
    int maiorA = 0;
    int indexA = 0;
    int contador;
    //int valor = 21;
    //int teste = 18;
    namedWindow("Camera", CV_WINDOW_AUTOSIZE );
    //namedWindow("Mao", CV_WINDOW_AUTOSIZE );
    //createTrackbar("Mao","Mao",&teste,500);



    //Início do laço de captura da câmera.
    while (true) {
        camera.read(frame);
        if (!frame.empty()) {
            char c = (char) waitKey(1);
            if (c == 27) {

                break;
            } else if (c == 32){
                play = true;
                numero = std::rand()%6 + 1;
                win = (numero + contador)%2;
                //cout << win << endl;

            }
            cv::flip(frame,frame,1);
            cv::cvtColor(frame,hsv,COLOR_BGR2HSV);
            mao = hsv(rect);

            //SEGMENTANDO A IMAGEM EM 2 RANGES PARA A COR DA PELE
            cv::inRange(mao,Scalar(0,30,0),Scalar(20,255,255),primeira);
            cv::inRange(mao,Scalar(170,30,0),Scalar(180,255,255),segunda);
            mao = primeira|segunda;

            cv::dilate(mao,mao,kernel,Point(-1,-1),2);
            cv::GaussianBlur(mao,mao,Size(5,5),0);

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
            contador = 0;

            for (int i = 0; i < convDef[0].size(); i++){
                float depth = convDef[0][i][3]/256.0;
                if (depth > 18){
                    int id = convDef[0][i][2];
                    cv::circle(frame,contours[indexA][id],5,cv::Scalar(0,255,0),-1);
                    contador++;
                }
            }

            cv::rectangle(frame, rect, cv::Scalar(0, 255, 0),2);

            //Colocando informações textuais na tela
            putText(frame,std::to_string(contador), Point2f(150,350), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
            putText(frame,std::to_string(contador), Point2f(150,350), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);

            if (play){
                putText(frame,"PC: " + std::to_string(numero), Point2f(400,50), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
                putText(frame,"PC: " + std::to_string(numero), Point2f(400,50), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);
                if (win == 0){
                putText(frame,"VENCEU", Point2f(250,440), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
                putText(frame,"VENCEU", Point2f(250,440), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);
                } else {
                    putText(frame,"PERDEU", Point2f(250,440), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
                    putText(frame,"PERDEU", Point2f(250,440), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);
                }
            }

            imshow("Camera",frame);
            //imshow("Mao",mao);

        }
    }
    //Libera o recurso da câmera.
    camera.release();
    //Fecha todas as janelas.
    destroyAllWindows();

    return EXIT_SUCCESS;
}


