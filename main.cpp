#include <cstdlib>
#include <handle3ddataset.h>


int main(int argc, char **argv)
{
	int option_index = 0;
	DATAINFO img1Info;

	if (argc != 6)
	{
		printf("Falta argumentos");
		return -1;
	}

	img1Info.inputFileName= argv[1];
	img1Info.resWidth	  = atoi(argv[2]);
	img1Info.resHeight 	  = atoi(argv[3]);
	img1Info.initStack 	  = atoi(argv[4]);
	img1Info.endStack     = atoi(argv[5]);
	img1Info.resDepth     = img1Info.endStack - img1Info.initStack;

	printf("%s:[%dx%dx%d]\n", img1Info.inputFileName,img1Info.resWidth,img1Info.resHeight,img1Info.resDepth);
	
	Handle3DDataset <char>d1(img1Info);

	d1.loadFile();

	char **data1 = d1.getDataset();
	char *data2 = data1[0];


	printf("#########Img2D\n");
	for (int i = 0; i < img1Info.resWidth; ++i)
	{
		for (int j = 0; j < img1Info.resHeight; ++j)
		{
			printf("%d ",data2[i*img1Info.resWidth+j]);					
		}
		printf("\n");
	}
	printf("\n\n\n");

	char *subImg;
	
	int kernel = 3; //tamanho do kernel
	int pBase = (kernel * 2) + 1;
	int offset = kernel; // o offset minimo é o tamanho do kernel
	subImg = (char*)malloc(sizeof(char)* pBase*pBase);//sub imagens
	

	#define ijn(a,b,n) ((a)*(n))+b
	
	for (int i = offset; i < img1Info.resWidth-offset; i++) //percorre imagem pixel //coluna
	{
		for (int j = offset; j < img1Info.resHeight-offset; j++) //a pixel //linha
		{	
			for(int ii=0; ii < pBase; ii++)
			{
				for(int jj=0; jj < pBase; jj++)
				{
					int pData2 = ijn(i-kernel+ii, j-kernel+jj ,img1Info.resWidth);
					subImg[ijn(ii, jj, pBase)] = data2[pData2];
					printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
				}
				printf("\n");
			}
			printf("\n\n\n");
		}
	}	

	printf("##########Img3D\n");
	for (int k = img1Info.initStack; k < img1Info.endStack; ++k)
	{
		for (int i = 0; i < img1Info.resWidth; ++i)
		{
			for (int j = 0; j < img1Info.resHeight; ++j)
			{
				printf("%d ",data1[k][i*img1Info.resWidth+j]);
			}
			printf("\n");
		}
		printf("\n\n");
	}

	printf("####SUB\n");
	for (int k = 0; k < img1Info.resDepth; k++)
	{
		for (int i = offset; i < img1Info.resWidth-offset; i++) //percorre imagem pixel //coluna
		{
			for (int j = offset; j < img1Info.resHeight-offset; j++) //a pixel //linha
			{	
				for(int ii=0; ii < pBase; ii++)
				{
					for(int jj=0; jj < pBase; jj++)
					{
						int pData2 = ijn(i-kernel+ii, j-kernel+jj ,img1Info.resWidth);
						subImg[ijn(ii, jj, pBase)] = data1[k][pData2];
						printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
					}
					printf("\n");
				}
				printf("\n\n\n");
			}
		}
		printf("################\n");
	}


	// char **subImg;

	// subImg = (char**)malloc(4 * sizeof(char*)); 
	// for (int k = 0; k < 4; ++k) //4 sub imagens
	// {
	// 	subImg[k] = (char*)malloc(sizeof(char)* 8*8);//sub imagens de 8x8
	// }

	// printf("SubImg1\n");

	// int wTamanho = 4;
	// int hTamanho = 4;



	// for (int i = 0; i < img1Info.resWidth; ++i)
	// {
	// 	for (int j = 0; j < img1Info.resHeight; ++j)
	// 	{
	// 		printf("%d ",data2[i*img1Info.resWidth+j]);					
	// 	}
	// }
	// printf("\n\n");



	// for (int k = 0; k < 4; ++k)
	// {
	// 	for (int i = wInitSubImage; i < wEndSubImage; ++i)
	// 	{
	// 		for (int j = hInitSubImage; j < hEndSubImage; ++j)
	// 		{
	// 			printf("k[%d],%d,%d   ",k,i*8+j,i*wEndSubImage+j);
	// 			subImg[k][i*8+j] = data2[i*wEndSubImage+j];
	// 		}
	// 		//wInitSubImage=
	// 	}
	// 	wInitSubImage+=wEndSubImage;
	// 	hInitSubImage+=hEndSubImage;
	// 	wEndSubImage+=8;
	// 	hEndSubImage+=8;
	// }

	// for (int k = 0; k < 4; ++k)
	// {
	// 	for (int i = 0; i < 8; ++i)
	// 	{
	// 		for (int j = 0; j < 8; ++j)
	// 		{
	// 			printf("%d ",subImg[k][i*8+j]);
	// 		}
	// 	}
	// 	printf("\n\n");
	// }

	


	return 0;
}