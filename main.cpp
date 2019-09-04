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
    Mat mascaraPele;
    Mat mao1;
    Mat mao2;


    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    //Kernel utilizado para as operações morfológicas.
    Mat kernel;
    kernel = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

    int x = 10;
    int y = 10;
    int width = 450;
    int height = 500;
    cv::Rect rect(x, y, width, height);
    x = 815;
    cv::Rect rect2(x, y, width, height);

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
        cvtColor(frame, imagemHSV, COLOR_BGR2HSV);

        //SEGMENTANDO A IMAGEM EM 2 RANGES PARA A COR DA PELE
        inRange(imagemHSV,  Scalar(0, 30, 0), Scalar(20, 255, 255), primeiraFaixa);
        inRange(imagemHSV, Scalar(170, 30, 0), Scalar(180, 255, 255), segundaFaixa);
        mascaraPele = primeiraFaixa | segundaFaixa;

        // Operador morfologico de dilatação
        dilate(mascaraPele, mascaraPele, kernel, Point(-1,-1), 2);

        //Passa baixa para remoção de ruidos
        GaussianBlur(mascaraPele, mascaraPele, Size(3, 3), 0);

        //Cortando a parte selecionada para mão 1 e 2
        mao1 = mascaraPele(rect2);
        mao2 = mascaraPele(rect);

        findContours(mao2, contours, hierarchy,CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE );
        maiorA = 0;
        indexA = 0;
        for (int i = 0; i < contours.size(); i++){
            double a = contourArea(contours[i],false);
            if (a > maiorA){
                maiorA = a;
                indexA = i;
            }
        }
        vector<vector<Point> >hull(1);
        convexHull(Mat(contours[indexA]), hull[0], false );

        drawContours(frame, hull, (int)0, Scalar(255,255,0), 1, 8, vector<Vec4i>(), 0, Point() );

        //Exibindo as areas das mãos
        cv::rectangle(frame, rect, cv::Scalar(0, 255, 0),2);
        cv::rectangle(frame, rect2, cv::Scalar(0, 255, 0),2);

        //Invertendo a imagem para uma melhor visualização
        cv::flip(frame,frame,1);
        cv::flip(mascaraPele,mascaraPele,1);

        //Colocando informações textuais na tela
        putText(frame,"JOGADOR 1", Point2f(150,550), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
        putText(frame,"JOGADOR 2", Point2f(950,550), FONT_HERSHEY_PLAIN, 2,  Scalar(0,0,0), 4);
        putText(frame,"JOGADOR 1", Point2f(150,550), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);
        putText(frame,"JOGADOR 2", Point2f(950,550), FONT_HERSHEY_PLAIN, 2,  Scalar(255,255,255), 1);

        //Exibindo imagem
        imshow("Entrada",frame);
        imshow("Mao 2", mao1);
        imshow("Mao 1", mao2);

        }
    }
    //Libera o recurso da câmera.
    camera.release();
    //Fecha todas as janelas.
    destroyAllWindows();

    return EXIT_SUCCESS;
}


