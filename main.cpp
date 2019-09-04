#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

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
    Mat imagemHSV;
    Mat primeiraFaixa;
    Mat segundaFaixa;
    Mat mao1;
    Mat mao2;


    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //Kernel utilizado para as operações morfológicas.
    Mat kernel;
    kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

    cv::Rect rect1(10, 10, 450, 500);
    cv::Rect rect2(815, 10, 450, 500);

    int maiorA = 0;
    int indexA = 0;

    //Início do laço de captura da câmera.
    while (true) {
        camera >> frame;
        if (!frame.empty()) {
            char c = (char) waitKey(1);
            if (c == 27) {
                break;
            }

        //Invertendo a imagem para uma melhor visualização
        cv::flip(frame,frame,1);
        cvtColor(frame, imagemHSV, COLOR_BGR2HSV);

        //Cortando a parte selecionada para mão 1 e 2
        mao1 = imagemHSV(rect1);
        mao2 = imagemHSV(rect2);

        //SEGMENTANDO A IMAGEM EM 2 RANGES PARA A COR DA PELE
        inRange(mao2,  Scalar(0, 30, 0), Scalar(20, 255, 255), primeiraFaixa);
        inRange(mao2, Scalar(170, 30, 0), Scalar(180, 255, 255), segundaFaixa);
        mao2 = primeiraFaixa | segundaFaixa;

        inRange(mao1,  Scalar(0, 30, 0), Scalar(20, 255, 255), primeiraFaixa);
        inRange(mao1, Scalar(170, 30, 0), Scalar(180, 255, 255), segundaFaixa);
        mao1 = primeiraFaixa | segundaFaixa;

        // Operador morfologico de dilatação
        dilate(mao2, mao2, kernel, Point(-1,-1), 2);
        dilate(mao1, mao1, kernel, Point(-1,-1), 2);

        //Passa baixa para remoção de ruidos
        blur(mao2,mao2,Size(3,3));
        blur(mao1, mao1, Size(3,3));

        findContours(mao1, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
        maiorA = 0;
        indexA = 0;
        for (int i = 0; i < contours.size(); i++){
            double a = contourArea(contours[i],false);
            if (a > maiorA){
                maiorA = a;
                indexA = i;
            }
        }


        std::vector<std::vector<int> > hullsI(2);
        std::vector<std::vector<cv::Vec4i> >convDef(2);

        cv::convexHull(cv::Mat(contours[indexA]), hullsI[0], false, true);
        cv::convexityDefects(contours[indexA], hullsI[0], convDef[0]);
        for (int j = 0; j < convDef[0].size(); ++j){
            //float depth = convDef[0][j][3]/256;
            if (convDef[0][j][3] > 25*256 /*filter defects by depth*/){
                int ind_0 = convDef[0][j][0];//start point
                int ind_1 = convDef[0][j][1];//end point
                int ind_2 = convDef[0][j][2];//defect point
                cv::circle(frame, contours[indexA][ind_0], 5, cv::Scalar(0, 0, 255), -1);
                cv::circle(frame, contours[indexA][ind_1], 5, cv::Scalar(255, 0, 0), -1);
                cv::circle(frame, contours[indexA][ind_2], 5, cv::Scalar(0, 255, 0), -1);
            }
        }



        //Exibindo as areas das mãos
        cv::rectangle(frame, rect1, cv::Scalar(0, 255, 0),2);
        cv::rectangle(frame, rect2, cv::Scalar(0, 255, 0),2);

        //Colocando informações textuais na tela
        putText(frame,"JOGADOR 1", Point2f(150,550), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
        putText(frame,"JOGADOR 2", Point2f(950,550), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
        putText(frame,"JOGADOR 1", Point2f(150,550), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);
        putText(frame,"JOGADOR 2", Point2f(950,550), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);

        //Exibindo imagem
        imshow("Entrada",frame);
        //imshow("Jogador 2", mao2);
        //imshow("Jogador 1", mao1);

        }
    }
    //Libera o recurso da câmera.
    camera.release();
    //Fecha todas as janelas.
    destroyAllWindows();

    return EXIT_SUCCESS;
}


