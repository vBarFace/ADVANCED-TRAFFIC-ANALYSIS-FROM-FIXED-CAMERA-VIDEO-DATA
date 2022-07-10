// main.cpp

#include <opencv2/core/core.hpp>			/* A biblioteca principal do OpenCV, cont�m todas as estruturas de dados e
										as fun��es b�sicas para trabalhar em imagens */
#include <opencv2/highgui/highgui.hpp>	/* Processamento de Imagem, para aplicar um filtro a uma imagem ou
										uma transforma��o geom�trica */
#include <opencv2/imgproc/imgproc.hpp>	// Conjunto de fun��es para I/O e para exibi��o de imagens ou v�deos em janelas

#include <iostream>			/* A biblioteca padr�o que inclui tudo relacionado � sa�da e entrada de v�deo
							teclado, gerenciamento de arquivos, estruturas de dados e algoritmos b�sicos */
#include <conio.h>           // Crea interfacce testuali e modifica il colore del testo nell'output

#include "blob.h"			// Arquivo de cabe�alho criado no arquivo blob.h

#include <fstream>			// Biblioteca para exportar dados para um arquivo de texto

// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

#include <map> // Para podermos usar maps (dicionarios)

#include <math.h> // Para podemos usar formulas matem�ticas

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// VARI�VEIS GLOBAIS /////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);			// Cria uma constante de valor BLACK codificada em RGB
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);	// Cria uma constante de valor WHITE codificada em RGB
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);		// Cria uma constante de valor YELLOW codificada em RGB
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);		// Cria uma constante de valor GREEN codificada em RGB
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);			// Cria uma constante de valor RED codificada em RGB
const cv::Scalar SCALAR_BLUE = cv::Scalar(255.0, 0.0, 0.0);         // Cria uma constante de valor BLUE codificada em RGB

/*Nesta zona armazenamos cores em forma de escalar numa vari�vel*/

// FUN��ES DE PROT�TIPOS ////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlobs, std::vector<Blob>& currentFrameBlobs);
void addBlobToExistingBlobs(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs, int& intIndex);
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
//bool checkIfBlobsCrossedTheLine(std::vector<Blob>& blobs, int& intHorizontalLinePosition, int& lightCount, int& heavyCount, int& j, int& k, cv::Mat& FrameCounter);
//bool checkIfBlobsCrossedTheLine2(std::vector<Blob>& blobs, int& intHorizontalLinePosition2, int& lightCount, int& heavyCount, int& j, int& k, cv::Mat& FrameCounter);
//bool checkIfBlobsCrossedTheLine3(std::vector<Blob>& blobs, int& intHorizontalLinePosition3, int& lightCount, int& heavyCount, int& j, int& k, cv::Mat& FrameCounter);
void drawBlobInfoOnImage(std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy);
void drawCarCountOnImage(int& lightCount, int& heavyCount, cv::Mat& imgFrame2Copy);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

void drawAvgSpeedOnImage(float& avgSpeed, cv::Mat& imgFrame2Copy, int& Blob_ID);
float VSPcalculationLigeiros(float& speed, float& acceleration);
std::vector<float> VSPcalculationHeavy(float& speed, float& acceleration, float& distanceTraveled);
std::vector<float> VSPcomparatorLight(float& VSPvalue, int& gasType);
std::vector<float> VSPcomparatorHeavy(float& VSPvalue);

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
/*
A primeira fun��o verifica se uma nova massa cont�nua que apareceu no frame est� a uma dist�ncia suficiente das j� existentes
A segunda fun��o incorpora a nova massa continua para o mais pr�ximo se a dist�ncia for menor que o valor m�nimo
A terceira fun��o torna a nova massa cont�nua independente, associando-a a um centro de gravidade pr�prio
A quarta fun��o calcula a dist�ncia entre dois pontos
A quinta e sexta fun��es desenham os contornos, as massas cont�nuas, etc.
A s�tima fun��o verifica se uma massa cont�nua passa pela primeira linha horizontal
A oitava fun��o verifica se uma massa cont�nua passa pela segunda linha horizontal
A nona fun��o desenha o ret�ngulo e o centro de gravidade de cada massa cont�nua no v�deo
A d�cima fun��o desenha o contador do ve�culo no v�deo
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {					// Fun��o main do programa

	cv::VideoCapture capVideo;		// Classe usada para capturar v�deo do arquivo

	cv::Mat imgFrame1;				// Criar uma matriz de nome imgFrame1 (� o background ?)
	cv::Mat imgFrame2;				// Criar uma matriz de nome imgFrame2 (Frame para sacar os carros)

	std::vector<Blob> blobs;		// Cria o vetor dos blobs

	// S�o as linhas de interce��o

	cv::Point crossingLine[2];		// Cria um vetor contendo dois pontos (onde a linha horizontal come�a/termina) -- 1� linha
	cv::Point crossingLine2[2];		// 2� linha
	cv::Point crossingLine3[2];		// 3� linha

	int lightCount = 0;				// Inicia a vari�vel que vai guardar a contagem de ve�culos ligieros como zero
	int heavyCount = 0;				// Inicia a vari�vel que vai guardar a contagem de ve�culos pesados como zero

	std::ofstream output("Output.txt");		// Criar um ficheiro de texto

	 capVideo.open("24C_N235_13_21_29.mp4");	// Fun��o que abre o ficheiro de v�deo resolucao_do_problema_linha_detecao
	//capVideo.open("11C_N235_12_07_30.mp4");
	//capVideo.open("17C_N235_12_46_29.mp4");
	//capVideo.open("10C_N235_12_02_30.mp4");
	//capVideo.open("03C_N235_11_27_30.mp4");
	//capVideo.open("teste_autocaravana.mp4"); // Resolver o erro da autocaravana
	//capVideo.open("resolucao_do_problema_linha_detecao.mp4"); // Para resolver o problema da dete��o de linhas
	//capVideo.open("teste_da_overview_projeto_v2.mp4");	// Contagens a mais
	//capVideo.open("teste_contagem_maisUM.mp4");	// Para testar a overview do video
	//capVideo.open("TesteDaParticula.mp4"); // Paara testar a particula, a dete��o da particula
	//capVideo.open("TesteDaMota.mp4"); // Para ignorar motas
	//capVideo.open("camiaoTeste.mp4");	//Teste do camiao
	//capVideo.open("08C_N235_11_52_30.mp4"); // Para testar com pesados e com um gajo contra a m�o 11C_N235_12_07_30
	//capVideo.open("teste_dos_dois_camioes.mp4");
	/* Condi��es de erro*/

	if (!capVideo.isOpened()) {														// Se o v�deo n�o abrir
		std::cout << "Errore nell'apertura del video" << std::endl << std::endl;    // Mostra a mensagem de erro
		_getch();																	// Aguarda que uma tecla seja pressionada
		return(0);																	// Sai do programa
	}

	if (capVideo.get(CV_CAP_PROP_FRAME_COUNT) < 2) {								// Se o v�deo tiver menos de dois frames
		std::cout << "Errore: Il video deve avere almeno due frames";				// Mostra a mensagem de erro
		_getch();																	// Aguarda que uma tecla seja pressionada
		return(0);																	// Sai do programa
	}

	float FPS = 0;																	// Frames por segundo inicializado como zero
	FPS = capVideo.get(CV_CAP_PROP_FPS);											// Ir buscar o FPS do v�deo e guardar em FPS
	std::cout << "Frames per second (FPS) = " << FPS;								// Escrever FPS no ficheiro de texto criado em cima
	std::cout << "\n";																// Escrever um enter no ficheiro de texto criado em cima
	cv::String q = "Frames per second (FPS) = ";										// Definir a string "Fotogrammi per secondo = "
	cv::String u = "\n";															// Definir a string "\n"
	output << q << FPS << u << std::endl;											// Escrever no ficheiro de texto FPS � frente de q

	capVideo.read(imgFrame1);		// Fun��o que vai buscar o pr�ximo frame do v�deo
	capVideo.read(imgFrame2);		// Fun��o que vai buscar o pr�ximo frame do v�deo


	int intHorizontalLinePosition = (int)std::round((double)imgFrame1.cols * 0.235);	// Posi��o da primeira linha horizontal de dete��o y - DEFAULT *0.27

	/*Ponto de come�o (valor um) da 1� linha de dete��o*/
	crossingLine[0].x = 400;							// Comprimento da primeira linha (valor um)
	crossingLine[0].y = intHorizontalLinePosition;		// Altura da primeira linha (valor um) 

	/*Ponto de t�rmino (valor dois) da 1� linha de dete��o*/
	crossingLine[1].x = (imgFrame1.cols - 600);			// Comprimento da primeira linha (valor dois)
	crossingLine[1].y = intHorizontalLinePosition;		// Altura da primeira linha (valor dois)

	int intHorizontalLinePosition2 = (int)std::round((double)imgFrame1.cols * 0.27);	// Posi��o da segunda linha horizontal de dete��o y - DEFAULT *0.32

	crossingLine2[0].x = 350;							// Comprimento da segunda linha (valor um)
	crossingLine2[0].y = intHorizontalLinePosition2;	// Altura da segunda linha (valor um)

	crossingLine2[1].x = (imgFrame1.cols - 600);		// Comprimento da segunda linha (valor dois)
	crossingLine2[1].y = intHorizontalLinePosition2;	// Altura da segunda linha (valor dois)

	int intHorizontalLinePosition3 = (int)std::round((double)imgFrame1.cols * 0.32);	// Posi��o da terceira linha horizontal de dete��o y - DEFAULT *0.40

	crossingLine3[0].x = 250;							// Comprimento da terceira linha (valor um)
	crossingLine3[0].y = intHorizontalLinePosition3;	// Altura da terceira linha (valor um)

	crossingLine3[1].x = (imgFrame1.cols - 600);		// Comprimento da terceira linha (valor dois)
	crossingLine3[1].y = intHorizontalLinePosition3;	// Altura da terceira linha (valor dois)


	char chCheckForEscKey = 0;		// Inicializar o char para sair do programa

	bool blnFirstFrame = true;		// Inicializa a vari�vel booleana para a compara��o dos blobs que entram em frame

	int frameCount = 2;				// Inicialize o contador de frames

	//int j = 1;						// �ndice de ve�culos leves
	//int k = 1;						// �ndice de ve�culos pesados

	//cv::Mat FrameCounter;			// Define a matriz que cont�m o n�mero do frame atual

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

	// Aqui � feita a cria��o de um hashmap/dicionario com o intuito de ser possivel guardar os frames em que o blob foi detetado. Est�o associados a uma chave �nica
	std::map <int, std::vector<int> > AllVehiclesDetectionFrames;
	int blob_ID_count = 1;

	// Mapa para contar ve�culos
	std::map <int, int> AllVehiclesCounting;

	// Mapas para guaradar as emiss�es
	std::map <int, std::vector<float>> VSPlight; // Primeiro valor � o VSPvalue, VSPmode, CO2, CO, NOx e HC
	std::map <int, std::vector<float>> VSPheavy; // Primeiro valor � o VSPvalue, CO2, VSPmode, CO, NOx e HC

	// Defini��o de vari�veis que tem associadas as dist�ncias entre linhas de dete��o
	int dist_red_white = 4;
	int dist_white_blue = 4;

	std::cout << "\nDistance (in meters) between red and white detection lines :" << dist_red_white;
	std::cout << "\nDistance (in meters) between white and blue detection lines :" << dist_white_blue << "\n";
	output << "\nDistance (in meters) between red and white detection lines :" << dist_red_white;
	output << "\nDistance (in meters) between white and blue detection lines :" << dist_white_blue << "\n" << std::endl;

	// O necess�rio para fazer o calculo da velocidade m�dia atual de todo o tr�fego, a medida que os ve�culos vao passando, � feita a m�dia da velocidade
	std::vector<float> AllVehiclesMeanVelocity;

	// Definir a vari�vel para guardar a velocidade m�dia do tr�fego atual para imprimir no v�deo
	float CurrentAverageSpeed = 0;

	// Defini��o de um mapa que tem como key o blob_ID
	std::map <int, int> carType; // Se for 0 � ligeiro, se for 1 � pesado e se for -1 nao � nenhum dos dois

	// Cria��o de vari�veis para contar quantos ligieors de gasolina e gasolio existem
	int petrolCount = 0;
	int dieselCount = 0;

	// Mapa para armazenar o gastype dos ligeiros
	std::map<int, int> gasTypeLight;

	// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

	while (capVideo.isOpened() && chCheckForEscKey != 27) {		// Enquanto o v�deo est� aberto e a tecla clicada seja diferente de "ESC" o ciclo corre

		std::vector<Blob> currentFrameBlobs;	// Vetor do blob do frame atual

		cv::Mat imgFrame1Copy = imgFrame1.clone();		// Define a c�pia da matriz imgFrame1
		cv::Mat imgFrame2Copy = imgFrame2.clone();		// Define a c�pia da matriz imgFrame2

		cv::Mat imgDifference;		// Define uma matriz de "diferen�a"
		cv::Mat imgThresh;			// Define uma matriz de "threshold", de limites

		cv::cvtColor(imgFrame1Copy, imgFrame1Copy, CV_BGR2GRAY);		// Torna o primeiro frame preto e branco, imagem bin�ria
		cv::cvtColor(imgFrame2Copy, imgFrame2Copy, CV_BGR2GRAY);		// Torna o segundo frame preto e branco, imagem bin�ria

		cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(3, 3), 0);		// Desfoque gaussiano (Gaussian Blur) do primeiro frame, serve para reduzir o ru�do da imagem
		cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(3, 3), 0); 		// Desfoque gaussiano (Gaussian Blur) do segundo frame, serve para reduzir o ru�do da imagem

		cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgDifference);		// Calcule a diferen�a entre os frames e guarda na vari�vel 'imgDifference'

		cv::threshold(imgDifference, imgThresh, 30, 255.0, CV_THRESH_BINARY);	/*Esta fun��o estabelece um valor limite e torna todos os pixels com
																				um valor menor que o limite pretos e os outros brancos*/
		//cv::imshow("imgDifference", imgDifference);
		//cv::imshow("imgThresh", imgThresh);		// Mostrar a janela que foi convertida para preto e branco

		cv::Mat structuringElement3x3 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));		// Criar uma matriz 3x3
		cv::Mat structuringElement4x4 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(4, 4));		// Criar uma matriz 4x4
		cv::Mat structuringElement5x5 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));		// Criar uma matriz 5x5
		cv::Mat structuringElement6x6 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(6, 6));		// Criar uma matriz 6x6
		cv::Mat structuringElement7x7 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 7));		// Criar uma matriz 7x7
		cv::Mat structuringElement15x15 = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(15, 15));	// Criar uma matriz 15x15

		for (unsigned int i = 0; i < 7; i++) {							// Morphological Transformation -- Tranforma��o morfol�gica
			cv::dilate(imgThresh, imgThresh, structuringElement6x6);	// Esticar o frame, torna o blob da imagem bin�ria mais gordo, neste caso as matrizes de limites, aumenta a area branca
			cv::dilate(imgThresh, imgThresh, structuringElement6x6);	// Esticar o frame, torna o blob da imagem bin�ria mais gordo, neste caso as matrizes de limites, aumenta a area branca
			cv::erode(imgThresh, imgThresh, structuringElement6x6);		// Corr�i o frame, torna o blob da imagem bin�ria mais magro, neste caso as matrizes de limites, diminuia a area branca
		}

		cv::Mat imgThreshCopy = imgThresh.clone();	// � feita uma c�pia da matriz dos limites, threshold matrix

		std::vector<std::vector<cv::Point> > contours;	// � definido um vetor de contornos do blob, a boundering box

		cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);	// Encontra os contornos do carro atrav�s da fun��o 'findContours'

		//drawAndShowContours(imgThresh.size(), contours, "imgContours");	// Mostrar uma janela com contornos desenhados, a boundering box, os blobs

		std::vector<std::vector<cv::Point> > convexHulls(contours.size());	// � definido um vetor da regi�o convexa, uma boundinggBox mais bonitinha

		for (unsigned int i = 0; i < contours.size(); i++)	// Para cada elemento do contorno
			cv::convexHull(contours[i], convexHulls[i]);	// Calculo da regi�o convexa do elemento do contorno

		//drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");	// Mostrar a janela com as regi�es convexas, blobs bonitinhos

		for (auto& convexHull : convexHulls) {	// Para cada regi�o convexa 
			Blob possibleBlob(convexHull);		// � encontrado o poss�vel blob que corresponde � regi�o convexa

			if (possibleBlob.currentBoundingRect.area() > 400 &&	// Ret�ngulo / area do associada ao poss�vel blob, o area() faz parte da class Rect
				possibleBlob.dblCurrentAspectRatio > 0.2 &&			// Propor��o m�nima do associada ao poss�vel blob
				possibleBlob.dblCurrentAspectRatio < 4.0 &&			// Propor��o m�xima do associada ao poss�vel blob
				possibleBlob.currentBoundingRect.width > 30 &&		// Largura do blob do associada ao poss�vel blob
				possibleBlob.currentBoundingRect.height > 30 &&		// Altura do blob do associada ao poss�vel blob
				possibleBlob.dblCurrentDiagonalSize > 60.0 &&		// Diagonal do blob 
				(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {

				currentFrameBlobs.push_back(possibleBlob);

			}
			/* � feita uma compara��o com os valores selecionados: a �rea da boundingBox que o cont�m e as propor��es, largura e a altura
			do boundingBox que o cont�m, sua diagonal e a raz�o entre a �rea de fronteira e a �rea da bounderingBox
			Se esses crit�rios forem atendidos, o blob n�o � o mais "prov�vel", mas sim o blob definitivo, o correto, ent�o e acrescentado ao array de blobs em frame */
		}

		// Mostra as janelas com os blobs do frame atual, um ap�s o outro
		//drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

		if (blnFirstFrame == true) {								// Se o blob aparece no frame pela primeira vez
			for (auto& currentFrameBlob : currentFrameBlobs) {		// Para cada blob em frame

				blobs.push_back(currentFrameBlob);	// O n�mero de blobs definitivos � aumentado, � acrescentado ao vetor 'blobs' atrav�s da 'push_back'

			}
		}
		else {																	// Se o blob j� estiver presente no frame
			matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);	// O blob que j� apareceu em frame � colocado numa nova posi��o


		}

		//drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");	// Mostrar a janela com os blobs de forma "cont�nua"

		imgFrame2Copy = imgFrame2.clone();	// � feita uma c�pia do segundo frame inicial, para aparecer a cores

		drawBlobInfoOnImage(blobs, imgFrame2Copy);	// � desenhado o centroide do ve�culo � bounding box

		bool blnAtLeastOneBlobCrossedTheLine = false;	// � inicializada a vari�vel que deteta a travessia pela 1� linha de dete��o

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

		bool blnAtLeastOneBlobCrossedTheLine2 = false;	// � inicializada a vari�vel que deteta a travessia pela 2� linha de dete��o
		bool blnAtLeastOneBlobCrossedTheLine3 = false;	// � inicializada a vari�vel que deteta a travessia pela 3� linha de dete��o

		cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);	// Desenho da primeira linha horizontal cujas posi��es foram definidas em cima
		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		cv::line(imgFrame2Copy, crossingLine2[0], crossingLine2[1], SCALAR_WHITE, 2);	// Desenho da segunda linha horizontal cujas posi��es foram definidas em cima
		cv::line(imgFrame2Copy, crossingLine3[0], crossingLine3[1], SCALAR_BLUE, 2);    // Desenho da terceira linha horizontal cujas posi��es foram definidas em cima

		for (auto blob : blobs) {	// Para cada blob em cada frame
			// Se o blob tem um centroide (e � grande o suficiente), a sua posi��o � determinada no frame anterior e no atual
			if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
				int prevFrameIndex = (int)blob.centerPositions.size() - 2;
				int currFrameIndex = (int)blob.centerPositions.size() - 1;

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

				if (AllVehiclesDetectionFrames[blob_ID_count].size() != 3) {

					AllVehiclesDetectionFrames[blob_ID_count].push_back(-1);	// Primeira linha de dete��o
					AllVehiclesDetectionFrames[blob_ID_count].push_back(-1);	// Segunda linha de dete��o
					AllVehiclesDetectionFrames[blob_ID_count].push_back(-1);	// Terceira linha de dete��o

					carType[blob_ID_count] = -1;
				}

				// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				// Se o centroide no frame anterior for mais baixo (estiver a baixo) e no frame seguinte for mais alto (estiver a cima) que a primeira linha de dete��o
				if (blob.centerPositions[prevFrameIndex].y <= intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y > intHorizontalLinePosition) {
					// A primeira linha 
					if (blob.dblCurrentDiagonalSize > 150 && blob.dblCurrentDiagonalSize < 450) {	// Se tiver uma diagonal pequena, � ligeiro

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

						AllVehiclesDetectionFrames[blob_ID_count][0] = frameCount; // Guaradamoos o frame de dete��o da primeira linha

						if (AllVehiclesDetectionFrames[blob_ID_count][0] != -1) { // Se tocou na primeira linha

							carType[blob_ID_count] = 0;
						}

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					}
					else if (blob.dblCurrentDiagonalSize >= 450) {	// Caso nao seja uma diagonal pequena, se h� uma diagonal grande, � pesado																	

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

						AllVehiclesDetectionFrames[blob_ID_count][0] = frameCount;	// Guaradamoos o frame de dete��o da primeira linha

						if (AllVehiclesDetectionFrames[blob_ID_count][0] != -1) { // Se tocar na primeira linha

							carType[blob_ID_count] = 1;
						}

					}
					else {	// Nem � pesado nem � ligiero

						carType[blob_ID_count] = -1;

					}

					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					blnAtLeastOneBlobCrossedTheLine = true; // Atribui o valor TRUE � vari�vel que leva em considera��o a passagem na linha
				}

				// Repita o processo que est� em cima mas agora para a segunda linha
				if (blob.centerPositions[prevFrameIndex].y <= intHorizontalLinePosition2 && blob.centerPositions[currFrameIndex].y > intHorizontalLinePosition2) {
					if (blob.dblCurrentDiagonalSize > 180 && blob.dblCurrentDiagonalSize < 450) { // � ligeiro

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

						AllVehiclesDetectionFrames[blob_ID_count][1] = frameCount;

						if (AllVehiclesDetectionFrames[blob_ID_count][0] == -1 && AllVehiclesDetectionFrames[blob_ID_count][1] != -1) {	// Se nao tocar na primeira linha e tocar na segunda

							carType[blob_ID_count] = 0;
						}

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					}
					else if (blob.dblCurrentDiagonalSize >= 448) {	// � pesado

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

						AllVehiclesDetectionFrames[blob_ID_count][1] = frameCount;

						if (AllVehiclesDetectionFrames[blob_ID_count][0] == -1 && AllVehiclesDetectionFrames[blob_ID_count][1] != -1) {

							carType[blob_ID_count] = 1;
						}

					}
					else {	// Nem � pesado nem � ligiero

						carType[blob_ID_count] = -1;

					}

					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

					blnAtLeastOneBlobCrossedTheLine2 = true; // Se tocar na segunda linha

					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				}

				// Repita o processo que est� em cima mas agora para a terceira linha
				if (blob.centerPositions[prevFrameIndex].y <= intHorizontalLinePosition3 && blob.centerPositions[currFrameIndex].y > intHorizontalLinePosition3) {
					if (blob.dblCurrentDiagonalSize > 230 && blob.dblCurrentDiagonalSize < 450) { // � ligeiro

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

						AllVehiclesDetectionFrames[blob_ID_count][2] = frameCount;

						if (AllVehiclesDetectionFrames[blob_ID_count][0] == -1 && AllVehiclesDetectionFrames[blob_ID_count][1] == -1 && AllVehiclesDetectionFrames[blob_ID_count][2] != -1) {	// Se nao tocar na primeira linha nem na segunda mas tocar na terceira

							carType[blob_ID_count] = 0;
						}

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

					}
					else if (blob.dblCurrentDiagonalSize >= 450) { // � pesado

						// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

						AllVehiclesDetectionFrames[blob_ID_count][2] = frameCount;

						if (AllVehiclesDetectionFrames[blob_ID_count][0] == -1 && AllVehiclesDetectionFrames[blob_ID_count][1] == -1 && AllVehiclesDetectionFrames[blob_ID_count][2] != -1) {

							carType[blob_ID_count] = 1;
						}

					}
					else {	// Nem � pesado nem � ligiero

						carType[blob_ID_count] = -1;

					}

					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
					// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

					blnAtLeastOneBlobCrossedTheLine3 = true; // Se tocar na terceira linha, que vai sempre acontecer

					if (AllVehiclesDetectionFrames[blob_ID_count][0] == AllVehiclesDetectionFrames[blob_ID_count][1] || AllVehiclesDetectionFrames[blob_ID_count][1] == AllVehiclesDetectionFrames[blob_ID_count][2] || AllVehiclesDetectionFrames[blob_ID_count][0] == AllVehiclesDetectionFrames[blob_ID_count][2]) {

						continue; // Em situa��es que h� dete��o em duas linhas ao mm tempo, pode acontecer por causa do reshape dasbounding boxes

					}
					// Caso das tr�s linhas serem ativadas
					else if (AllVehiclesDetectionFrames[blob_ID_count][0] != -1 && AllVehiclesDetectionFrames[blob_ID_count][1] != -1 && AllVehiclesDetectionFrames[blob_ID_count][2] != -1) {

						// Impress�o do map no terminal
						if (carType[blob_ID_count] == 0) { // Se for ligiero

							int gasType = rand() % 2; // Cria um n�mero alet�rio, ou 0 ou 1.
							gasTypeLight[blob_ID_count] = gasType;

							if (gasTypeLight[blob_ID_count] == 0) { // Se for igual a 0 � gasolina

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";

								petrolCount++;

							}
							else if (gasTypeLight[blob_ID_count] == 1) { // Se for igual a 1 � gasoleo

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";

								dieselCount++;

							}
							else {

								std::cout << "\nERROR";
								break;

							}

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";

							// Calculo dos frames viajados entre as 3 linhas
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][1] - AllVehiclesDetectionFrames[blob_ID_count][0];	// Frames viajados entre a 1� e 2� linha
							int lineDiff2 = AllVehiclesDetectionFrames[blob_ID_count][2] - AllVehiclesDetectionFrames[blob_ID_count][1];	// Frames viajados entre a 2� e 3� linha

							std::cout << "\nFrames traveled between the 1st and 2nd detection lines: " << lineDiff1;
							std::cout << "\nFrames traveled between the 2nd and 3th detection lines: " << lineDiff2;

							// Obten��o do tempo de viagem em segundos
							float timeTraveled1 = (lineDiff1 / FPS);	// Calculo do tempo de viagem entre a 1� e 2� linha
							float timeTraveled2 = (lineDiff2 / FPS);	// Calculo do tempo de viagem entre a 2� e 3� linha

							std::cout << "\nTime spent (in seconds) between the 1st and 2nd detection lines: " << timeTraveled1;
							std::cout << "\nTime spent (in seconds) between the 2nd and 3th detection lines: " << timeTraveled2;

							// Calculo da velocidade entre as 3 linhas da� tiramos a velocidade m�dia
							float velocity1 = dist_red_white / timeTraveled1;	// Velocidade entre a 1� e 2� linha
							float velocity2 = dist_white_blue / timeTraveled2;	// Velocidade entre a 2� e 3� linha
							float velocityMean = (velocity1 + velocity2) / 2;	// M�dia das velocidade entre as duas sec��es
							float velocityMean_KMh = (velocityMean * 3600) / 1000; // Convers�o de m/s para km/h

							std::cout << "\nVelocity/Speed between the 1st and 2nd detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 2nd and 3th detection lines: " << velocity2 << "(m/s)";
							std::cout << "\nMean Velocity/Speed: " << velocityMean << "(m/s)";
							std::cout << "\nMean Velocity/Speed: " << velocityMean_KMh << "(km/h)";
							output << "\nMean Velocity/Speed (km/h): " << velocityMean_KMh;

							AllVehiclesMeanVelocity.push_back(velocityMean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego

							// Calculo da acelera��o
							float acceleration = (velocity2 - velocity1) / (timeTraveled2 - timeTraveled1);

							if (velocity1 == velocity2 || timeTraveled1 == timeTraveled2) { // Sgnifica que a acelera��o � constante, entao metemos como 0
								acceleration = 0;
							}

							float accelarationKMh2 = acceleration * 12960;	// 12960 = ((60*60)^2)/1000 = (3600^2)/1000

							std::cout << "\nAcceleration: " << acceleration << "(m/s^2)";
							std::cout << "\nAcceleration: " << accelarationKMh2 << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nAcceleration: " << accelarationKMh2 << "(km/h^2)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para ligeiros
							float VSPvalueLigiero = VSPcalculationLigeiros(velocityMean, acceleration);
							VSPlight[blob_ID_count].push_back(VSPvalueLigiero);

							std::cout << "\nVSP value: " << VSPlight[blob_ID_count][0];
							output << "\nVSP value: " << VSPlight[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para ligeiros
							std::vector<float> modeEmissionsLight = VSPcomparatorLight(VSPvalueLigiero, gasTypeLight[blob_ID_count]);
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[0]);	// Metemos o VSPmode
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[1]);	// Metemos o CO2
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[2]);	// Metemos o CO
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[3]);	// Metemos o NOx
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[4]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							output << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							std::cout << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementado em um

							lightCount++;
							AllVehiclesCounting[0] = lightCount;

						}
						else if (carType[blob_ID_count] == 1) { // Se for pesado

							std::cout << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";
							output << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";


							// Calculo dos frames viajados entre as 3 linhas
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][1] - AllVehiclesDetectionFrames[blob_ID_count][0];	// Frames viajados entre a 1� e 2� linha
							int lineDiff2 = AllVehiclesDetectionFrames[blob_ID_count][2] - AllVehiclesDetectionFrames[blob_ID_count][1];	// Frames viajados entre a 2� e 3� linha

							std::cout << "\nFrames traveled between the 1st and 2nd detection lines: " << lineDiff1;
							std::cout << "\nFrames traveled between the 2nd and 3th detection lines: " << lineDiff2;

							// Obten��o do tempo de viagem em segundos
							float timeTraveled1 = (lineDiff1 / FPS);	// Calculo do tempo de viagem entre a 1� e 2� linha
							float timeTraveled2 = (lineDiff2 / FPS);	// Calculo do tempo de viagem entre a 2� e 3� linha

							std::cout << "\nTime spent (in seconds) between the 1st and 2nd detection lines: " << timeTraveled1;
							std::cout << "\nTime spent (in seconds) between the 2nd and 3th detection lines: " << timeTraveled2;

							// Calculo da velocidade entre as 3 linhas da� tiramos a velocidade m�dia
							float velocity1 = dist_red_white / timeTraveled1;	// Velocidade entre a 1� e 2� linha
							float velocity2 = dist_white_blue / timeTraveled2;	// Velocidade entre a 2� e 3� linha
							float velocityMean = (velocity1 + velocity2) / 2;	// M�dia das velocidade entre as duas sec��es
							float velocityMean_KMh = (velocityMean * 3600) / 1000; // Convers�o de m/s para km/h

							std::cout << "\nVelocity/Speed between the 1st and 2nd detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 2nd and 3th detection lines: " << velocity2 << "(m/s)";
							std::cout << "\nMean Velocity/Speed: " << velocityMean << "(m/s)";
							std::cout << "\nMean Velocity/Speed: " << velocityMean_KMh << "(km/h)";
							output << "\nMean Velocity/Speed (km/h): " << velocityMean_KMh;

							AllVehiclesMeanVelocity.push_back(velocityMean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego

							// Calculo da acelera��o
							float acceleration = (velocity2 - velocity1) / (timeTraveled2 - timeTraveled1);

							if (velocity1 == velocity2 || timeTraveled1 == timeTraveled2) { // Sgnifica que a acelera��o � constante, entao metemos como 0
								acceleration = 0;
							}

							float accelarationKMh2 = acceleration * 12960; // 12960 = ((60*60)^2)/1000 = (3600^2)/1000

							std::cout << "\nAcceleration: " << acceleration << "(m/s^2)";
							std::cout << "\nAcceleration: " << accelarationKMh2 << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nAcceleration: " << accelarationKMh2 << "(km/h^2)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para Pesados
							float total_distance = dist_red_white + dist_white_blue;
							std::vector<float> VSPvalueHeavy = VSPcalculationHeavy(velocityMean, acceleration, total_distance);
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[0]); // O valor do VSP
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[1]); // O valor do CO2

							std::cout << "\nVSP value: " << VSPheavy[blob_ID_count][0];
							output << "\nVSP value: " << VSPheavy[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para pesados
							std::vector<float> modeEmissionsHeavy = VSPcomparatorHeavy(VSPheavy[blob_ID_count][0]);
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[0]);	// Metemos o VSPmode
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[1]);	// Metemos o CO
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[2]);	// Metemos o NOx
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[3]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPheavy[blob_ID_count][2];
							output << "\nVSP mode: " << VSPheavy[blob_ID_count][2];
							std::cout << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementaado em um

							heavyCount++;
							AllVehiclesCounting[1] = heavyCount;

						}
						else {			// Se nem for pesado nem ligeiro -- carType[blob_ID_count][0] == -1 && carType[blob_ID_count][1] == -1
							continue;	
						}

					}
					// Caso da 1� e 2� linha
					else if (AllVehiclesDetectionFrames[blob_ID_count][0] != -1 && AllVehiclesDetectionFrames[blob_ID_count][1] != -1 && AllVehiclesDetectionFrames[blob_ID_count][2] == -1) {

						// Impress�o do map no terminal
						if (carType[blob_ID_count] == 0) { // Ser ligeiro

							int gasType = rand() % 2; // Cria um n�mero alet�rio, ou 0 ou 1.
							gasTypeLight[blob_ID_count] = gasType;

							if (gasTypeLight[blob_ID_count] == 0) { // Se for igual a 0 � gasolina

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";

								petrolCount++;

							}
							else if (gasTypeLight[blob_ID_count] == 1) { // Se for igual a 1 � gasoleo

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";

								dieselCount++;

							}
							else {

								std::cout << "\nERROR";

							}

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";

							// Frames viajados entre a 1� e 2� linha
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][1] - AllVehiclesDetectionFrames[blob_ID_count][0];
							std::cout << "\nFrames traveled between the 1st and 2nd detection lines: " << lineDiff1;

							// Calculo do tempo de viagem entre a 1� e 2� linha
							float timeTraveled1 = (lineDiff1 / FPS);
							std::cout << "\nTime spent (in seconds) between the 1st and 2nd detection lines: " << timeTraveled1;

							// Velocidade entre a 1� e 2� linha
							float velocity1 = dist_red_white / timeTraveled1;
							float velocity1Mean_KMh = (velocity1 * 3600) / 1000; // Convers�o de m/s para km/h
							std::cout << "\nVelocity/Speed between the 1st and 2nd detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 1st and 2nd detection lines: " << velocity1Mean_KMh << "(km/h)";
							output << "\nVelocity/Speed : " << velocity1Mean_KMh << "(km/h)";


							AllVehiclesMeanVelocity.push_back(velocity1Mean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego


							float acceleration = 0;
							output << "\nAcceleration: " << acceleration << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para ligeiros
							float VSPvalueLigiero = VSPcalculationLigeiros(velocity1, acceleration);
							VSPlight[blob_ID_count].push_back(VSPvalueLigiero);

							std::cout << "\nVSP value: " << VSPlight[blob_ID_count][0];
							output << "\nVSP value: " << VSPlight[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para ligeiros
							std::vector<float> modeEmissionsLight = VSPcomparatorLight(VSPvalueLigiero, gasTypeLight[blob_ID_count]);
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[0]);	// Metemos o VSPmode
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[1]);	// Metemos o CO2
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[2]);	// Metemos o CO
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[3]);	// Metemos o NOx
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[4]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							output << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							std::cout << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementaado em um

							lightCount++;
							AllVehiclesCounting[0] = lightCount;

						}
						else if (carType[blob_ID_count] == 1) { // Ser Pesado

							std::cout << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";
							output << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";

							// Frames viajados entre a 1� e 2� linha
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][1] - AllVehiclesDetectionFrames[blob_ID_count][0];
							std::cout << "\nFrames traveled between the 1st and 2nd detection lines: " << lineDiff1;

							// Calculo do tempo de viagem entre a 1� e 2� linha
							float timeTraveled1 = (lineDiff1 / FPS);
							std::cout << "\nTime spent (in seconds) between the 1st and 2nd detection lines: " << timeTraveled1;

							// Velocidade entre a 1� e 2� linha
							float velocity1 = dist_red_white / timeTraveled1;
							float velocity1Mean_KMh = (velocity1 * 3600) / 1000; // Convers�o de m/s para km/h
							std::cout << "\nVelocity/Speed between the 1st and 2nd detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 1st and 2nd detection lines: " << velocity1Mean_KMh << "(km/h)";
							output << "\nVelocity/Speed : " << velocity1Mean_KMh << "(km/h)";


							AllVehiclesMeanVelocity.push_back(velocity1Mean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego


							float acceleration = 0;
							output << "\nAcceleration: " << acceleration << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para Pesados
							float total_distance = dist_red_white;
							std::vector<float> VSPvalueHeavy = VSPcalculationHeavy(velocity1, acceleration, total_distance);
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[0]); // O valor do VSP
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[1]); // O valor do CO2

							std::cout << "\nVSP value: " << VSPheavy[blob_ID_count][0];
							output << "\nVSP value: " << VSPheavy[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para pesados
							std::vector<float> modeEmissionsHeavy = VSPcomparatorHeavy(VSPheavy[blob_ID_count][0]);
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[0]);	// Metemos o VSPmode
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[1]);	// Metemos o CO
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[2]);	// Metemos o NOx
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[3]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPlight[blob_ID_count][2];
							output << "\nVSP mode: " << VSPlight[blob_ID_count][2];
							std::cout << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementaado em um

							heavyCount++;
							AllVehiclesCounting[1] = heavyCount;

						}
						else {			// Se nem for pesado nem ligeiro -- carType[blob_ID_count][0] == -1 && carType[blob_ID_count][1] == -1
							continue;
						}

					}
					// Caso da 1� e 3� linha
					else if (AllVehiclesDetectionFrames[blob_ID_count][0] != -1 && AllVehiclesDetectionFrames[blob_ID_count][1] == -1 && AllVehiclesDetectionFrames[blob_ID_count][2] != -1) {

						// Impress�o do map no terminal
						if (carType[blob_ID_count] == 0) {	// Ser ligeiro

							int gasType = rand() % 2; // Cria um n�mero alet�rio, ou 0 ou 1.
							gasTypeLight[blob_ID_count] = gasType;

							if (gasTypeLight[blob_ID_count] == 0) { // Se for igual a 0 � gasolina

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";

								petrolCount++;

							}
							else if (gasTypeLight[blob_ID_count] == 1) { // Se for igual a 1 � gasoleo

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";

								dieselCount++;

							}
							else {

								std::cout << "\nERROR";

							}

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";

							// Frames viajados entre a 1� e 3� linha
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][2] - AllVehiclesDetectionFrames[blob_ID_count][0];
							std::cout << "\nFrames traveled between the 1st and 3th detection lines: " << lineDiff1;

							// Calculo do tempo de viagem entre a 1� e 3� linha
							float timeTraveled1 = (lineDiff1 / FPS);
							std::cout << "\nTime spent (in seconds) between the 1st and 3th detection lines: " << timeTraveled1;

							// Velocidade entre a 1� e 3� linha
							float velocity1 = (dist_red_white + dist_white_blue) / timeTraveled1;
							float velocity1Mean_KMh = (velocity1 * 3600) / 1000; // Convers�o de m/s para km/h
							std::cout << "\nVelocity/Speed between the 1st and 3th detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 1st and 3th detection lines: " << velocity1Mean_KMh << "(km/h)";
							output << "\nVelocity/Speed: " << velocity1Mean_KMh << "(km/h)";


							AllVehiclesMeanVelocity.push_back(velocity1Mean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego

							float acceleration = 0;
							output << "\nAcceleration: " << acceleration << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para ligeiros
							float VSPvalueLigiero = VSPcalculationLigeiros(velocity1, acceleration);
							VSPlight[blob_ID_count].push_back(VSPvalueLigiero);

							std::cout << "\nVSP value: " << VSPlight[blob_ID_count][0];
							output << "\nVSP value: " << VSPlight[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para ligeiros
							std::vector<float> modeEmissionsLight = VSPcomparatorLight(VSPvalueLigiero, gasTypeLight[blob_ID_count]);
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[0]);	// Metemos o VSPmode
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[1]);	// Metemos o CO2
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[2]);	// Metemos o CO
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[3]);	// Metemos o NOx
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[4]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							output << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							std::cout << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementaado em um

							lightCount++;
							AllVehiclesCounting[0] = lightCount;

						}
						else if (carType[blob_ID_count] == 1) {	// Ser pesado

							std::cout << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";
							output << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";

							// Frames viajados entre a 1� e 3� linha
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][2] - AllVehiclesDetectionFrames[blob_ID_count][0];
							std::cout << "\nFrames traveled between the 1st and 3th detection lines: " << lineDiff1;

							// Calculo do tempo de viagem entre a 1� e 3� linha
							float timeTraveled1 = (lineDiff1 / FPS);
							std::cout << "\nTime spent (in seconds) between the 1st and 3th detection lines: " << timeTraveled1;

							// Velocidade entre a 1� e 3� linha
							float velocity1 = (dist_red_white + dist_white_blue) / timeTraveled1;
							float velocity1Mean_KMh = (velocity1 * 3600) / 1000; // Convers�o de m/s para km/h
							std::cout << "\nVelocity/Speed between the 1st and 3th detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 1st and 3th detection lines: " << velocity1Mean_KMh << "(km/h)";
							output << "\nVelocity/Speed: " << velocity1Mean_KMh << "(km/h)";


							AllVehiclesMeanVelocity.push_back(velocity1Mean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego

							float acceleration = 0;
							output << "\nAcceleration: " << acceleration << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para Pesados
							float total_distance = dist_red_white + dist_white_blue;
							std::vector<float> VSPvalueHeavy = VSPcalculationHeavy(velocity1, acceleration, total_distance);
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[0]); // O valor do VSP
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[1]); // O valor do CO2

							std::cout << "\nVSP value: " << VSPheavy[blob_ID_count][0];
							output << "\nVSP value: " << VSPheavy[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para pesados
							std::vector<float> modeEmissionsHeavy = VSPcomparatorHeavy(VSPheavy[blob_ID_count][0]);
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[0]);	// Metemos o VSPmode
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[1]);	// Metemos o CO
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[2]);	// Metemos o NOx
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[3]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPheavy[blob_ID_count][2];
							output << "\nVSP mode: " << VSPheavy[blob_ID_count][2];
							std::cout << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementaado em um

							heavyCount++;
							AllVehiclesCounting[1] = heavyCount;

						}
						else {			// Se nem for pesado nem ligeiro -- carType[blob_ID_count][0] == -1 && carType[blob_ID_count][1] == -1
							continue;
						}

					}
					// Caso da 2� e 3� linha
					else if (AllVehiclesDetectionFrames[blob_ID_count][0] == -1 && AllVehiclesDetectionFrames[blob_ID_count][1] != -1 && AllVehiclesDetectionFrames[blob_ID_count][2] != -1) {

						// Impress�o do map no terminal
						if (carType[blob_ID_count] == 0) { // Se for ligeiro

							int gasType = rand() % 2; // Cria um n�mero alet�rio, ou 0 ou 1.
							gasTypeLight[blob_ID_count] = gasType;

							if (gasTypeLight[blob_ID_count] == 0) { // Se for igual a 0 � gasolina

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Petrol";

								petrolCount++;

							}
							else if (gasTypeLight[blob_ID_count] == 1) { // Se for igual a 1 � gasoleo

								std::cout << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";
								output << "\nCar ID: " << blob_ID_count << ", Light Vehicle - Diesel";

								dieselCount++;

							}
							else {

								std::cout << "\nERROR";

							}

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";

							// Frames viajados entre a 2� e 3� linha
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][2] - AllVehiclesDetectionFrames[blob_ID_count][1];
							std::cout << "\nFrames traveled between the 2nd and 3th detection lines: " << lineDiff1;

							// Calculo do tempo de viagem entre a 2� e 3� linha
							float timeTraveled1 = (lineDiff1 / FPS);
							std::cout << "\nTime spent (in seconds) between the 2nd and 3th detection lines: " << timeTraveled1;;

							// Velocidade entre a 2� e 3� linha
							float velocity1 = (dist_white_blue) / timeTraveled1;
							float velocity1Mean_KMh = (velocity1 * 3600) / 1000; // Convers�o de m/s para km/h
							std::cout << "\nVelocity/Speed between the 2nd and 3th detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 2nd and 3th detection lines: " << velocity1Mean_KMh << "(km/h)";
							output << "\nVelocity/Speed: " << velocity1Mean_KMh << "(km/h)";


							AllVehiclesMeanVelocity.push_back(velocity1Mean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego

							float acceleration = 0;
							output << "\nAcceleration: " << acceleration << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para ligeiros
							float VSPvalueLigiero = VSPcalculationLigeiros(velocity1, acceleration);
							VSPlight[blob_ID_count].push_back(VSPvalueLigiero);

							std::cout << "\nVSP value: " << VSPlight[blob_ID_count][0];
							output << "\nVSP value: " << VSPlight[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para ligeiros
							std::vector<float> modeEmissionsLight = VSPcomparatorLight(VSPvalueLigiero, gasTypeLight[blob_ID_count]);
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[0]);	// Metemos o VSPmode
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[1]);	// Metemos o CO2
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[2]);	// Metemos o CO
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[3]);	// Metemos o NOx
							VSPlight[blob_ID_count].push_back(modeEmissionsLight[4]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							output << "\nVSP mode: " << VSPlight[blob_ID_count][1];
							std::cout << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPlight[blob_ID_count][2] << "(g/s); CO = " << VSPlight[blob_ID_count][3] << "(mg/s); NOx = " << VSPlight[blob_ID_count][4] << "(mg/s); HC = " << VSPlight[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementaado em um

							lightCount++;
							AllVehiclesCounting[0] = lightCount;

						}
						else if (carType[blob_ID_count] == 1) { // Se for pesado

							std::cout << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";
							output << "\nCar ID: " << blob_ID_count << ", Heavy Vehicle";

							std::cout << "\nDetection Frames [";
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][0] << ", ";	// Frame de deta��o da primeira linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][1] << ", ";	// Frame de deta��o da segunda linha
							std::cout << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";	// Frame de deta��o da terceira linha
							output << "\nDetection Frames [" << AllVehiclesDetectionFrames[blob_ID_count][0] << ", " << AllVehiclesDetectionFrames[blob_ID_count][1] << ", " << AllVehiclesDetectionFrames[blob_ID_count][2] << "]";

							// Frames viajados entre a 1� e 3� linha
							int lineDiff1 = AllVehiclesDetectionFrames[blob_ID_count][2] - AllVehiclesDetectionFrames[blob_ID_count][1];
							std::cout << "\nFrames traveled between the 2nd and 3th detection lines: " << lineDiff1;

							// Calculo do tempo de viagem entre a 1� e 2� linha
							float timeTraveled1 = (lineDiff1 / FPS);
							std::cout << "\nTime spent (in seconds) between the 2nd and 3th detection lines: " << timeTraveled1;;

							// Velocidade entre a 2� e 3� linha
							float velocity1 = (dist_white_blue) / timeTraveled1;
							float velocity1Mean_KMh = (velocity1 * 3600) / 1000; // Convers�o de m/s para km/h
							std::cout << "\nVelocity/Speed between the 2nd and 3th detection lines: " << velocity1 << "(m/s)";
							std::cout << "\nVelocity/Speed between the 2nd and 3th detection lines: " << velocity1Mean_KMh << "(km/h)";
							output << "\nVelocity/Speed: " << velocity1Mean_KMh << "(km/h)";


							AllVehiclesMeanVelocity.push_back(velocity1Mean_KMh); // Guardamos a velocidade m�dia no vetor
							int sumVelocity = 0;
							for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

								sumVelocity += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

							}
							CurrentAverageSpeed = sumVelocity / blob_ID_count; // Calcula da velocidade m�dia do trafego

							float acceleration = 0;
							output << "\nAcceleration: " << acceleration << "(km/h^2)";
							std::cout << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";
							output << "\nCurrent average traffic Velocity/Speed: " << CurrentAverageSpeed << "(km/h)";

							// Calculo do VSP para Pesados
							float total_distance = dist_white_blue;
							std::vector<float> VSPvalueHeavy = VSPcalculationHeavy(velocity1, acceleration, total_distance);
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[0]); // O valor do VSP
							VSPheavy[blob_ID_count].push_back(VSPvalueHeavy[1]); // O valor do CO2

							std::cout << "\nVSP value: " << VSPheavy[blob_ID_count][0];
							output << "\nVSP value: " << VSPheavy[blob_ID_count][0];

							// Calculo do VSPmode e emissoes para pesados
							std::vector<float> modeEmissionsHeavy = VSPcomparatorHeavy(VSPheavy[blob_ID_count][0]);
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[0]);	// Metemos o VSPmode
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[1]);	// Metemos o CO
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[2]);	// Metemos o NOx
							VSPheavy[blob_ID_count].push_back(modeEmissionsHeavy[3]);	// Metemos o HC

							std::cout << "\nVSP mode: " << VSPheavy[blob_ID_count][2];
							output << "\nVSP mode: " << VSPheavy[blob_ID_count][2];
							std::cout << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";
							output << "\nEmissions: CO2 = " << VSPheavy[blob_ID_count][1] << "(g/s); CO = " << VSPheavy[blob_ID_count][3] << "(mg/s); NOx = " << VSPheavy[blob_ID_count][4] << "(mg/s); HC = " << VSPheavy[blob_ID_count][5] << "(mg/s)\n";

							blob_ID_count++; // O ID de cada blob � incrementaado em um

							heavyCount++;
							AllVehiclesCounting[1] = heavyCount;

						}
						else {			// Se nem for pesado nem ligeiro -- carType[blob_ID_count][0] == -1 && carType[blob_ID_count][1] == -1
							continue;	
						}

					}
					/*else if (AllVehiclesDetectionFrames[blob_ID_count][0] == AllVehiclesDetectionFrames[blob_ID_count][1] || AllVehiclesDetectionFrames[blob_ID_count][1] == AllVehiclesDetectionFrames[blob_ID_count][2]) {

						continue;

					}*/
					// Casos de dete��o de uma s� reta ou de nenhuma
					else {

						continue;

					}

					// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

				}

			}

		}

		if (blnAtLeastOneBlobCrossedTheLine == true) {										// Se tocar na primeira linha

			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);		// A primeira linha fica verde
		}
		else {

			cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);	// Desenhe a primeira linha vermelha porque nada foi detetado, e a cor original � vermelha
		}

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

		if (blnAtLeastOneBlobCrossedTheLine2 == true) { // Se tocar na segunda linha

			cv::line(imgFrame2Copy, crossingLine2[0], crossingLine2[1], SCALAR_GREEN, 2); // Metemos a verde

		}
		else { // Se nao tocar na segunda linha

			cv::line(imgFrame2Copy, crossingLine2[0], crossingLine2[1], SCALAR_WHITE, 2); // Metemos a branco

		}

		if (blnAtLeastOneBlobCrossedTheLine3 == true) {	// Se tocar na terceira linha

			cv::line(imgFrame2Copy, crossingLine3[0], crossingLine3[1], SCALAR_GREEN, 2);	// Metemos a verde

		}
		else {	// Se nao tocar na terceira linha 

			cv::line(imgFrame1Copy, crossingLine3[0], crossingLine3[1], SCALAR_BLUE, 2);	// Metemos a azul

		}

		drawAvgSpeedOnImage(CurrentAverageSpeed, imgFrame2Copy, blob_ID_count);

		// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

		drawCarCountOnImage(AllVehiclesCounting[0], AllVehiclesCounting[1], imgFrame2Copy); // Coloca o vehicle counter no v�deo

		cv::imshow("imgFrame2Copy", imgFrame2Copy);		// Mostrar o pr�ximo frame do v�deo

		//cv::waitKey(0);	// Permite visualizar o v�deo frame a frame

		// OUTRA ITERA��O

		currentFrameBlobs.clear();				// Remove os valores do vetor de blobs do frame anterior

		imgFrame1 = imgFrame2.clone();			// O segundo frame torna-se o primeiro na pr�xima itera��o (esta itera��o)

		if ((capVideo.get(CV_CAP_PROP_POS_FRAMES) + 1) < capVideo.get(CV_CAP_PROP_FRAME_COUNT)) {	// Se houver pelo menos um outro frame
			capVideo.read(imgFrame2);																// Esse frame � lido
		}
		else {

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%

			// Nesta sec��o fazemos uma mini overview do v�deo todo
			output << "\n\nFinal Overview\n";
			output << "\nNumber of Light Vehicles: " << AllVehiclesCounting[0];
			output << "\nNumber of Heavy Vehicles: " << AllVehiclesCounting[1];

			//int velocitySUMofALLvideo = 0;
			//int FinalAverageSpeed = 0;
			//for (int i = 0; i < AllVehiclesMeanVelocity.size(); i++) {

			//	velocitySUMofALLvideo += AllVehiclesMeanVelocity[i]; // Soma de todas as velocidades

			//}
			//FinalAverageSpeed = velocitySUMofALLvideo / blob_ID_count;

			output << "\nAverage Speed of the traffic: " << CurrentAverageSpeed << "(km/h)\n";

			// Para obter a m�dia do VSPmode tanto para os ligeiros como para os pesados
			float VSPmodeSumLight = 0;
			float VSPmodeSumHeavy = 0;
			float VSPmodeAVGlight = 0;
			float VSPmodeAVGheavy = 0;

			// Para obter a m�dia e o total de emiss�es para os ligieros
			float CO2_light_total = 0;
			float CO_light_total = 0;
			float NOx_light_total = 0;
			float HC_light_total = 0;

			float CO2_light_avg = 0;
			float CO_light_avg = 0;
			float NOx_light_avg = 0;
			float HC_light_avg = 0;

			// Para obter a m�dia e o total de emiss�es para os pesados
			float CO2_heavy_total = 0;
			float CO_heavy_total = 0;
			float NOx_heavy_total = 0;
			float HC_heavy_total = 0;

			float CO2_heavy_avg = 0;
			float CO_heavy_avg = 0;
			float NOx_heavy_avg = 0;
			float HC_heavy_avg = 0;


			for (std::map<int, std::vector<float> >::const_iterator itMAP_L = VSPlight.begin(); itMAP_L != VSPlight.end(); ++itMAP_L)
			{
				VSPmodeSumLight += itMAP_L->second[1]; // Vamos buscar o VSPmode a cada ve�culo ligiero

				CO2_light_total += itMAP_L->second[2];
				CO_light_total += itMAP_L->second[3];
				NOx_light_total += itMAP_L->second[4];
				HC_light_total += itMAP_L->second[5];

			}

			if (AllVehiclesCounting[0] > 0) {

				VSPmodeAVGlight = VSPmodeSumLight / AllVehiclesCounting[0];

				CO2_light_avg = CO2_light_total / AllVehiclesCounting[0];
				CO_light_avg = CO_light_total / AllVehiclesCounting[0];
				NOx_light_avg = NOx_light_total / AllVehiclesCounting[0];
				HC_light_avg = HC_light_total / AllVehiclesCounting[0];

			}
			else {

				VSPmodeAVGlight = 0;

				CO2_light_avg = 0;
				CO_light_avg = 0;
				NOx_light_avg = 0;
				HC_light_avg = 0;

			}


			for (std::map<int, std::vector<float> >::const_iterator itMAP_H = VSPheavy.begin(); itMAP_H != VSPheavy.end(); ++itMAP_H) // para iterar um mapa
			{
				VSPmodeSumHeavy += itMAP_H->second[1]; // Vamos buscar o VSPmode a cada ve�culo pesado

				CO2_heavy_total += itMAP_H->second[1];
				CO_heavy_total += itMAP_H->second[3];
				NOx_heavy_total += itMAP_H->second[4];
				HC_heavy_total += itMAP_H->second[5];

			}

			if (AllVehiclesCounting[1] > 0) {

				VSPmodeAVGheavy = VSPmodeSumHeavy / AllVehiclesCounting[1];

				CO2_heavy_avg = CO2_heavy_total / AllVehiclesCounting[1];
				CO_heavy_avg = CO_heavy_total / AllVehiclesCounting[1];
				NOx_heavy_avg = NOx_heavy_total / AllVehiclesCounting[1];
				HC_heavy_avg = HC_heavy_total / AllVehiclesCounting[1];

			}
			else {

				VSPmodeAVGheavy = 0;

				CO2_heavy_avg = 0;
				CO_heavy_avg = 0;
				NOx_heavy_avg = 0;
				HC_heavy_avg = 0;

			}

			output << "\nNumber of light vehciles moved by petrol: " << petrolCount;
			output << "\nNumber of heavy vehicles moved by diesel: " << dieselCount << "\n";

			output << "\nAverage VSP mode of the Light Vehicles: " << VSPmodeAVGlight;
			output << "\nAverage VSP mode of the Heavy Vehicles: " << VSPmodeAVGheavy << "\n";

			output << "\nTotal emissions of light vehicles: " << "CO2 = " << CO2_light_total << "(g/s), CO = " << CO_light_total << "(mg/s), NOx = " << NOx_light_total << "(mg/s), HC= " << HC_light_total << "(mg/s)";
			output << "\nAverage emissions of light vehicles: " << "CO2 = " << CO2_light_avg << "(g/s), CO = " << CO_light_avg << "(mg/s), NOx = " << NOx_light_avg << "(mg/s), HC= " << HC_light_avg << "(mg/s)\n";

			output << "\nTotal emissions of heavy vehicles: " << "CO2 = " << CO2_heavy_total << "(g/s), CO = " << CO_heavy_total << "(mg/s), NOx = " << NOx_heavy_total << "(mg/s), HC= " << HC_heavy_total << "(mg/s)";
			output << "\nAverage emissions of heavy vehicles: " << "CO2 = " << CO2_heavy_avg << "(g/s), CO = " << CO_heavy_avg << "(mg/s), NOx = " << NOx_heavy_avg << "(mg/s), HC= " << HC_heavy_avg << "(mg/s)\n";


			// Para obter os resultados de emissoes totais
			float CO2_total = 0;
			float CO_total = 0;
			float NOx_total = 0;
			float HC_total = 0;

			float CO2_total_avg = 0;
			float CO_total_avg = 0;
			float NOx_total_avg = 0;
			float HC_total_avg = 0;

			CO2_total = CO2_light_total + CO2_heavy_total;
			CO_total = CO_light_total + CO_heavy_total;
			NOx_total = NOx_light_total + NOx_heavy_total;
			HC_total = HC_light_total + HC_heavy_total;

			CO2_total_avg = CO2_total / (lightCount + heavyCount);
			CO_total_avg = CO_total / (lightCount + heavyCount);
			NOx_total_avg = NOx_total / (lightCount + heavyCount);
			HC_total_avg = HC_total / (lightCount + heavyCount);

			output << "\nTotal emissions of the traffic: " << "CO2 = " << CO2_total << "(g/s), CO = " << CO_total << "(mg/s), NOx = " << NOx_total << "(mg/s), HC= " << HC_total << "(mg/s)";
			output << "\nAverage emissions of the traffic: " << "CO2 = " << CO2_total_avg << "(g/s), CO = " << CO_total_avg << "(mg/s), NOx = " << NOx_total_avg << "(mg/s), HC= " << HC_total_avg << "(mg/s)";

			// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

			std::cout << "\nEnd of the video. Please consult the 'output' file created.\n";	// Caso contr�rio, mostra a seuinte string
			break;	// Sai do loop WHILE
		}

		blnFirstFrame = false;				// No segundo frame os blobs j� est�o presentes, logo a primeira vez que aparecem � false
		frameCount++;						// O contador marca um frame
		chCheckForEscKey = cv::waitKey(1);	// Aguarda a resposta do usu�rio, ou seja, que ele clique em alguma tecla
	}

	if (chCheckForEscKey != 27) {               // Se "ESC" n�o foi pressionado e o v�deo foi finalizado
		cv::waitKey(0);                         // A janela fica aberta, a consola
	}

	output.close();
	return(0);									// Retorna o valor zero para a fun��o principal, simboolizando que a fun��o foi concluida com sucesso
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o verifica se um novo blob que apareceu no frame est� a uma dist�ncia suficiente das j� existentes
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob>& existingBlobs, std::vector<Blob>& currentFrameBlobs) {

	for (auto& existingBlob : existingBlobs) {							// Para cada blob existente no frame

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;				// Inicializa a vari�vel que detecta novos blobs

		existingBlob.predictNextPosition();								// Chama a fun��o que prev� seu movimento
	}

	for (auto& currentFrameBlob : currentFrameBlobs) {					// Para cada blob no frame atual

		int intIndexOfLeastDistance = 0;								// Inicialize o contador de blobs
		double dblLeastDistance = 10000.0;								// Inicializa a dist�ncia m�nima entre dois blobs
		for (unsigned int i = 0; i < existingBlobs.size(); i++) {		// Para cada blob

			if (existingBlobs[i].blnStillBeingTracked == true) {		// Se um blob for detectado

				// A dist�ncia entre o centroide do blob e os centroides dos blobs existentes � calculada
				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {			// Se uma dessas dist�ncias for menor que o valor m�nimo
					dblLeastDistance = dblDistance;				// A distancia m�nima passa a ser a dist�ncia calculada a cima
					intIndexOfLeastDistance = i;				// O contador de blobs assume um novo valor
				}
			}
		}

		// Se a dist�ncia m�nima for menor um quarto da diagonal do blob atual
		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.25) {

			// Adiciona o blob atual aos existentes (de pleno direito com fun��o)
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);

		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);	// Caso contr�rio, adiciona um novo blob
		}

	}

	for (auto& existingBlob : existingBlobs) {								// Para cada blob existente

		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {			// Se o blob n�o for detetado em frame
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;			// O contador de frames sem dete��es aumenta em 1
		}

		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {		// Se este contador de blobs em frame exceder um determinado valor
			existingBlob.blnStillBeingTracked = false;						// O blob detatado detetado � ignorado
		}

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o incorpora o novo blob para o mais pr�ximo se a dist�ncia for menor que o valor m�nimo
void addBlobToExistingBlobs(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs, int& intIndex) {

	// Anexe um limite e um ret�ngulo para conter ao novo blob, isto �, basicamente acrescenta uma bounderingBox e os limites dessa boounderingBox
	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	// � adicionado e aumento o n�mero de blobs em um. Nota que � adicionado ao fim do array
	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	// Associar o novo blob com a sua diagonal e a raz�o entre as suas dimens�es, a mais longa e mais curta
	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnStillBeingTracked = true;				// O novo blob est� tracked
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;		// O novo blob � adicionado ao array dos blobs que ja tinham sido previamente tracked
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o torna o novo blob independente, associando-o a umcentroide pr�prio
void addNewBlob(Blob& currentFrameBlob, std::vector<Blob>& existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;		// Se um blob for detectada no frame atual

	existingBlobs.push_back(currentFrameBlob);					// � adicionado ao array
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o calcula a dist�ncia entre dois pontos, neste caso os centroides. Esta fun��o faz a distnacia euclidiana
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

	int intX = abs(point1.x - point2.x);					// Calcule a diferen�a entre as abcissas de dois pontos
	int intY = abs(point1.y - point2.y);					// Calcular a diferen�a entre as ordenadas de dois pontosi

	return(sqrt(pow(intX, 2) + pow(intY, 2)));				// Calcule a dist�ncia entre dois pontos
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Fun��o a ser ativada se quiseres mostrar as outras janelas na sa�da
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);	// Obter as informa��es da imagem

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);	// Desenha os contornos da imagem

	cv::imshow(strImageName, image);	// D� display � imagem numa janela j� existente
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Fun��o a ser ativada se quiseres mostrar as outras janelas na sa�da
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);	// Obter as informa��es da imagem

	std::vector<std::vector<cv::Point> > contours;	// Inicializa��o de um vetor de vetores, m array de arrays, para guardar dados

	for (auto& blob : blobs) {							// Para todos os blobs
		if (blob.blnStillBeingTracked == true) {		// Se estiver tracked
			contours.push_back(blob.currentContour);	// � adicionado ao array de countours
		}
	}

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);	// Desenha os contornos da imagem

	cv::imshow(strImageName, image);	// D� display � imagem numa janela j� existente
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//// A fun��o verifica se uma massa cont�nua passa pela primeira linha horizontal
//bool checkIfBlobsCrossedTheLine(std::vector<Blob>& blobs, int& intHorizontalLinePosition, int& lightCount, int& heavyCount, int& j, int& k, cv::Mat& FrameCounter) {
//	bool blnAtLeastOneBlobCrossedTheLine = false;	// Inicializa��o da vari�vel que verifica se houve algum blob a passar a primeira linha como falso
//
//	for (auto blob : blobs) {	// Para cada blob em frame
//
//		// Se o blob tem um centroide (e � grande o suficiente)
//		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
//			int prevFrameIndex = (int)blob.centerPositions.size() - 2;	// Determinar a posi��o do blob no frame anterior
//			int currFrameIndex = (int)blob.centerPositions.size() - 1;	// Determinar a posi��o do blob no frame atual
//
//			// Se o centroide no frame anterior for mais alto e no pr�ximo frame for mais baixo que a primeira linha, ou seja, antes e depois de cruzar a linha
//			if (blob.centerPositions[prevFrameIndex].y >= intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y < intHorizontalLinePosition) {
//
//				//if (blob.dblCurrentDiagonalSize < 500) {	// Se o tamanho da diagonal for menor que 500
//				//	lightCount++;			// Contamos um ve�culo ligeiro
//				//	FrameCounter.rows = j;	// Adicionamos mais um aos frames que detetaram os ve�culos ligeiros
//				//	j++;
//				//}
//				//else {	// Caso contr�rio
//				//	heavyCount++;	// Contamos um ve�culo pesado
//				//	FrameCounter.rows = k;	// Adicionamos mais um aos frames que detetaram os ve�culos pesados
//				//	k++;
//				//}
//				//if (FrameCounter.rows >= k) std::cout << "\nIndice veicoli leggeri = " << FrameCounter.rows;
//				//else std::cout << "\n				Indice veicoli pesanti = " << FrameCounter.rows;
//				blnAtLeastOneBlobCrossedTheLine = true;
//			}
//		}
//
//	}
//
//	return blnAtLeastOneBlobCrossedTheLine;
//}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////
//bool checkIfBlobsCrossedTheLine2(std::vector<Blob>& blobs, int& intHorizontalLinePosition2, int& lightCount, int& heavyCount, int& j, int& k, cv::Mat& FrameCounter) {
//	bool blnAtLeastOneBlobCrossedTheLine2 = false;
//
//	for (auto blob : blobs) {																// Per ogni massa continua nel frame
//
//		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
//			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
//			int currFrameIndex = (int)blob.centerPositions.size() - 1;
//			// Se la massa continua ha un baricentro (ed � grande abbastanza) determina la sua posizione nei due frames
//
//			if (blob.centerPositions[prevFrameIndex].y >= intHorizontalLinePosition2 && blob.centerPositions[currFrameIndex].y < intHorizontalLinePosition2) {
//				blnAtLeastOneBlobCrossedTheLine2 = true;
//			}
//
//		}
//
//	}
//
//	return blnAtLeastOneBlobCrossedTheLine2;
//	// Restituisce il valore TRUE se la massa continua attraversa la seconda linea orizzontale, FALSE altrimenti
//}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//bool checkIfBlobsCrossedTheLine3(std::vector<Blob>& blobs, int& intHorizontalLinePosition3, int& lightCount, int& heavyCount, int& j, int& k, cv::Mat& FrameCounter) {
//	bool blnAtLeastOneBlobCrossedTheLine3 = false;
//
//	for (auto blob : blobs) {																// Per ogni massa continua nel frame
//
//		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
//			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
//			int currFrameIndex = (int)blob.centerPositions.size() - 1;
//			// Se la massa continua ha un baricentro (ed � grande abbastanza) determina la sua posizione nei due frames
//
//			if (blob.centerPositions[prevFrameIndex].y >= intHorizontalLinePosition3 && blob.centerPositions[currFrameIndex].y < intHorizontalLinePosition3) {
//				blnAtLeastOneBlobCrossedTheLine3 = true;
//			}
//			// Se il baricentro nel frame precedente � pi� in alto e nel frame successivo � pi� in basso rispetto alla seconda linea
//
//			//if (blob.centerPositions[prevFrameIndex].y <= intHorizontalLinePosition2 && blob.centerPositions[currFrameIndex].y > intHorizontalLinePosition2) {
//				//blnAtLeastOneBlobCrossedTheLine2 = true;
//			//}
//			// Se il baricentro nel frame precedente � pi� in basso e nel frame successivo � pi� in alto rispetto alla seconda linea
//
//		}
//
//	}
//
//	return blnAtLeastOneBlobCrossedTheLine3;
//	// Restituisce il valore TRUE se la massa continua attraversa la seconda linea orizzontale, FALSE altrimenti
//}
///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o desenha a bounderingBox e o centroide de cada blob no v�deo
void drawBlobInfoOnImage(std::vector<Blob>& blobs, cv::Mat& imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {	// Para cada blob

		if (blobs[i].blnStillBeingTracked == true) {	// Se o blob esta detetado em frame
			if (blobs[i].dblCurrentDiagonalSize < 448 && blobs[i].dblCurrentDiagonalSize > 65) {

				cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_YELLOW, 2); // Desenha a bounderingBox amarela para ligeiros

			}
			else if (blobs[i].dblCurrentDiagonalSize >= 448) {

				cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_GREEN, 2);	// Desenha a bounderingBox verde para pesados

			}
			else {

				cv::rectangle(imgFrame2Copy, blobs[i].currentBoundingRect, SCALAR_RED, 2);	// Desenha a bounderingBox vermelha para o que nao interessa

			}
			int intFontFace = CV_FONT_HERSHEY_SIMPLEX;										// � definida a fonte do texto
			double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;					// � definido o tamanho do texto
			int intFontThickness = (int)std::round(dblFontScale * 1.0);						// � definida a espessura do texto

			cv::circle(imgFrame2Copy, blobs[i].centerPositions.back(), 5, SCALAR_RED, -1);	//  � desenhado o centroide do ve�culo

		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o desenha o contador do ve�culo no v�deo
void drawCarCountOnImage(int& lightCount, int& heavyCount, cv::Mat& imgFrame2Copy) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;										// � definida a fonte do texto
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;		// � definida o tamanho do texto
	int intFontThickness = (int)std::round(dblFontScale * 2);						// � definida a espessura do texto

	// S�o definas as caracter�sticas do texto destinado � contagem de ve�culos
	cv::Size textSize = cv::getTextSize(std::to_string(lightCount), intFontFace, dblFontScale, intFontThickness, 0);

	cv::Point ptTextBottomLeftPosition;	// Define o contador de ve�culos leves
	ptTextBottomLeftPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);	// Atribuir caracter�sticas ao texto destinado � contagem de ve�culos leves, ESPESSURA
	ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);	// Atribuir caracter�sticas ao texto destinado � contagem de ve�culos leves, ALTURA

	cv::Point ptTextBottomLeftPosition2;	// Define o contador de ve�culos pesados
	ptTextBottomLeftPosition2.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);	// Atribuir caracter�sticas ao texto destinado � contagem de ve�culos leves, ESPESSURA
	ptTextBottomLeftPosition2.y = (int)((double)textSize.height * 3);	// Atribuir caracter�sticas ao texto destinado � contagem de ve�culos leves, ESPESSURA

	cv::Mat img1;	// Define a matriz que conter� a imagem do ve�culo leve
	cv::Mat img2;	// Define a matriz que conter� a imagem do ve�culo pesado

	/*img1 = cv::imread("LIGHT.jpg");
	img1.copyTo(imgFrame2Copy(cv::Rect(655, 2.75, img1.cols, img1.rows)));*/
	// Inserisce l'immagine dei veicoli leggeri

	//img2 = cv::imread("HEAVY.jpg");
	//img2.copyTo(imgFrame2Copy(cv::Rect(655, 57.75, img2.cols, img2.rows)));
	// Inserisce l'immagine dei veicoli pesanti

	cv::putText(imgFrame2Copy, std::to_string(lightCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_YELLOW, intFontThickness);	// Inser o contador dos ve�culos dos ligieros
	cv::putText(imgFrame2Copy, std::to_string(heavyCount), ptTextBottomLeftPosition2, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);	// Inser o contador dos ve�culos dos pesados

}

// %%%%%%%%%%%%%%%%%%%%%%%%%%%% MINHAS IMPLEMENTA��ES %%%%%%%%%%%%%%%%%%%%%%%%%%%%
///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o desenha o contador da velocidade m�dia do tr�fego
void drawAvgSpeedOnImage(float& avgSpeed, cv::Mat& imgFrame2Copy, int& Blob_ID) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;										// � definida a fonte do texto
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;		// � definida o tamanho do texto
	int intFontThickness = (int)std::round(dblFontScale * 2);						// � definida a espessura do texto

	// S�o definas as caracter�sticas do texto destinado � contagem de ve�culos
	cv::Size textSize = cv::getTextSize(std::to_string(avgSpeed), intFontFace, dblFontScale, intFontThickness, 0);

	cv::Point ptTextBottomLeftPosition3;

	if (Blob_ID == 1) {

		ptTextBottomLeftPosition3.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 0.38);
	}
	else {

		ptTextBottomLeftPosition3.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 0.45);
	}

	ptTextBottomLeftPosition3.y = (int)((double)textSize.height * 4.75);

	cv::putText(imgFrame2Copy, cv::format("%.1f", avgSpeed), ptTextBottomLeftPosition3, intFontFace, dblFontScale, SCALAR_BLACK, intFontThickness);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o que vai Calcular o VSP dos ligeiros
float VSPcalculationLigeiros(float& speed, float& acceleration) {

	float VSP;

	// VSP para Carros e LDV (carrinhas, pickups, etc...)
	VSP = speed * (1.1 * acceleration + 0.132) + 0.000302 * pow(speed, 3);

	return VSP;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o que vai Calcular o VSP dos Pesados e a estimativa do valor de CO2
std::vector<float> VSPcalculationHeavy(float& speed, float& acceleration, float& distanceTraveled) {

	std::vector <float> VSP_CO2;
	float VSP;

	float mass = 15790; // kg
	float COPERT_CO2;

	VSP = speed * (acceleration + 0.186333) + 3.702456 / mass;
	VSP_CO2.push_back(VSP); // Primeiro indice do vetor � o VSP

	if (speed <= ((50 * 1000) / 3600)) {

		COPERT_CO2 = (0.385 * pow(speed, 2) - 37.037 * speed + 1424.6) * distanceTraveled;
		VSP_CO2.push_back(COPERT_CO2);// Segundo indice do vetor � o COPERT CO2

	}
	else if (speed > ((50 * 1000) / 3600)) {

		COPERT_CO2 = (0.0434 * pow(speed, 2) - 7.0232 * speed + 767.71) * distanceTraveled;
		VSP_CO2.push_back(COPERT_CO2);// Segundo indice do vetor � o COPERT CO2

	}

	return VSP_CO2;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// A fun��o que vai pegar no valor do VSP obtido e comparar com outros e obter o valor final para os Ligieros
std::vector<float> VSPcomparatorLight(float& VSPvalue, int& gasType) {

	int VSPmode = 0;
	std::vector<float> ModeAndEmissionsLight;
	float CO2;
	float CO;
	float NOx;
	float HC;

	if (gasType == 0) { // Se for gasolina

		if (VSPvalue < -2) {
			VSPmode = 1;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 1.04;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 2.25;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 0.31;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.29;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= -2 && VSPvalue < 0) {
			VSPmode = 2;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 1.31;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 2.88;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 0.41;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.35;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 0 && VSPvalue < 1) {
			VSPmode = 3;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 0.93;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 1.79;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 0.19;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.26;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 1 && VSPvalue < 4) {
			VSPmode = 4;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 2.17;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 4.96;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 0.82;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.56;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 4 && VSPvalue < 7) {
			VSPmode = 5;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 3.00;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 7.43;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 1.28;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.78;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 7 && VSPvalue < 10) {
			VSPmode = 6;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 3.77;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 9.30;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 1.81;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.96;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 10 && VSPvalue < 13) {
			VSPmode = 7;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 4.47;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 12.23;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 2.40;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 1.13;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 13 && VSPvalue < 16) {
			VSPmode = 8;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 5.05;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 14.38;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 2.90;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 1.32;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 16 && VSPvalue < 19) {
			VSPmode = 9;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 5.61;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 19.54;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 3.54;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 1.48;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 19 && VSPvalue < 23) {
			VSPmode = 10;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 6.01;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 22.31;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 3.98;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 1.59;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 23 && VSPvalue < 28) {
			VSPmode = 11;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 6.48;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 29.14;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 4.79;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 1.73;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 28 && VSPvalue < 33) {
			VSPmode = 12;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 6.96;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 36.73;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 5.49;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 1.86;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 33 && VSPvalue < 39) {
			VSPmode = 13;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 7.41;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 54.38;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 6.41;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 2.03;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 39) {
			VSPmode = 14;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 8.06;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 128.28;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 6.06;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 2.28;
			ModeAndEmissionsLight.push_back(HC);
		}
		else { // erro

			VSPmode = 0;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 0;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 0;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0;
			ModeAndEmissionsLight.push_back(HC);

		}
	}
	else if (gasType == 1) { // Se for diesel

		if (VSPvalue < -2) {
			VSPmode = 1;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 0.206;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.03;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 1.3;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.14;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= -2 && VSPvalue < 0) {
			VSPmode = 2;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 0.608;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.072;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 2.6;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.11;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 0 && VSPvalue < 1) {
			VSPmode = 3;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 0.728;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.135;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 3.4;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.11;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 1 && VSPvalue < 4) {
			VSPmode = 4;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 1.504;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.246;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 6.1;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.17;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 4 && VSPvalue < 7) {
			VSPmode = 5;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 2.343;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.29;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 9.4;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.2;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 7 && VSPvalue < 10) {
			VSPmode = 6;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 3.287;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.693;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 12.5;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.23;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 10 && VSPvalue < 13) {
			VSPmode = 7;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 4.2;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.584;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 15.5;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.24;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 13 && VSPvalue < 16) {
			VSPmode = 8;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 4.936;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.639;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 17.8;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.23;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 16 && VSPvalue < 19) {
			VSPmode = 9;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 5.574;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.61;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 21.3;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.24;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 19 && VSPvalue < 23) {
			VSPmode = 10;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 6.262;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 1;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 32.5;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.28;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 23 && VSPvalue < 28) {
			VSPmode = 11;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 7.396;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 1.1;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 55.8;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.37;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 28 && VSPvalue < 33) {
			VSPmode = 12;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 8.3898;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.9579;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 74.3;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.415;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 33 && VSPvalue < 39) {
			VSPmode = 13;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 9.4142;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.76841;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 104.2;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.4;
			ModeAndEmissionsLight.push_back(HC);
		}
		else if (VSPvalue >= 39) {
			VSPmode = 14;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = 10.4818;
			ModeAndEmissionsLight.push_back(CO2);
			CO = 0.73343;
			ModeAndEmissionsLight.push_back(CO);
			NOx = 145.9;
			ModeAndEmissionsLight.push_back(NOx);
			HC = 0.42;
			ModeAndEmissionsLight.push_back(HC);
		}
		else { // erro

			VSPmode = 0;
			ModeAndEmissionsLight.push_back(VSPmode);

			CO2 = -1;
			ModeAndEmissionsLight.push_back(CO2);
			CO = -1;
			ModeAndEmissionsLight.push_back(CO);
			NOx = -1;
			ModeAndEmissionsLight.push_back(NOx);
			HC = -1;
			ModeAndEmissionsLight.push_back(HC);

		}

	}

	return ModeAndEmissionsLight;

}

// A fun��o que vai pegar no valor do VSP obtido e comparar com outros e obter o valor final para os Pesados
std::vector<float> VSPcomparatorHeavy(float& VSPvalue) {

	int VSPmode = 0;
	std::vector<float> ModeAndEmissionsHeavy;
	float CO;
	float NOx;
	float HC;

	if (VSPvalue < -25) {
		VSPmode = 1;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 1.3250;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 7.1723;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 1.7616;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= -25 && VSPvalue < -20) {
		VSPmode = 2;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 0.9457;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 7.6521;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 2.6399;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= -20 && VSPvalue < -14) {
		VSPmode = 3;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 1.4794;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 8.7245;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 3.2946;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= -14 && VSPvalue < -10) {
		VSPmode = 4;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 1.3027;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 12.0206;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 3.3108;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= -10 && VSPvalue < -5) {
		VSPmode = 5;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 3.6174;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 6.4120;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 4.7910;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= -5 && VSPvalue < -2) {
		VSPmode = 6;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 5.1957;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 11.4792;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 6.9809;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= -2 && VSPvalue < -1) {
		VSPmode = 7;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 11.4613;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 33.8444;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 2.2593;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= -1 && VSPvalue < 0) {
		VSPmode = 8;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 7.9797;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 21.4202;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 1.0422;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 0 && VSPvalue < 1) {
		VSPmode = 9;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 9.9653;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 36.9548;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 0.9430;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 1 && VSPvalue < 2) {
		VSPmode = 10;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 15.1145;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 60.0636;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 3.0147;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 2 && VSPvalue < 5) {
		VSPmode = 11;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 23.8573;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 62.1429;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 2.9065;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 5 && VSPvalue < 10) {
		VSPmode = 12;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 37.2366;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 93.3285;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 4.6050;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 10 && VSPvalue < 14) {
		VSPmode = 13;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 41.1333;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 66.9862;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 3.1037;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 14 && VSPvalue < 20) {
		VSPmode = 14;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 24.9549;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 81.3605;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 3.5288;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 20 && VSPvalue < 25) {
		VSPmode = 15;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 12.5323;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 94.7466;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 2.7615;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else if (VSPvalue >= 25) {
		VSPmode = 16;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 16.6543;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 106.1606;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 3.1497;
		ModeAndEmissionsHeavy.push_back(HC);
	}
	else { // se der erro

		VSPmode = 0;
		ModeAndEmissionsHeavy.push_back(VSPmode);

		CO = 0;
		ModeAndEmissionsHeavy.push_back(CO);
		NOx = 0;
		ModeAndEmissionsHeavy.push_back(NOx);
		HC = 0;
		ModeAndEmissionsHeavy.push_back(HC);

	}

	return ModeAndEmissionsHeavy;

}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%