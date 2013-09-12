#include <cstdlib>
#include <handle3ddataset.h>
#include <fstream>
#include <ctime>

#define ijn(a,b,n) ((a)*(n))+b
#define V false

#define KERNEL 1
#define PBASE KERNEL*2+1
#define OFFSET KERNEL


void buildImagePlanes(int d, int w, int h, int img_w)
{
	if(V)printf("________AXIAL\n");
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+1,ijn(w-KERNEL+i,h-KERNEL+j,img_w));
		}
		if(V)printf("\n");	
	}
	if(V)printf("\n");

	if(V)printf("________DIAGONAL PRINCIPAL AXIAL\n");
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+i,ijn(w-KERNEL+j,h-KERNEL+j,img_w));
		}
		if(V)printf("\n");	
	}
	if(V)printf("\n");

	if(V)printf("______DIAGONAL SECUNDARIA AXIAL\n");
	for(int i = 0; i < PBASE; i++)
	{
		for (int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+i,ijn(w-KERNEL+j,h-KERNEL+PBASE-j-1,img_w));	
		}
		if(V)printf("\n");
	}
	if(V)printf("\n");

	if(V)printf("________CORONAL\n");
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+j,ijn(w-KERNEL+1,h-KERNEL+i,img_w));
		}
		if(V)printf("\n");	
	}
	if(V)printf("\n");

	if(V)printf("_______DIAGONAL PRINCIPAL CORONAL\n");
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+i,ijn(w-KERNEL+j,h-KERNEL+i,img_w));
		}
		if(V)printf("\n");	
	}
	if(V)printf("\n");

	if(V)printf("_______DIAGONAL SECUNDARIA CORONAL\n");
	for(int i = PBASE-1; i >= 0; i--)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+i,ijn(w-KERNEL+j,h-KERNEL+i,img_w));
		}
		if(V)printf("\n");	
	}
	if(V)printf("\n");

	if(V)printf("________SAGITTAL\n");
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+j,ijn(w-KERNEL+i,h-KERNEL+1,img_w));
		}
		if(V)printf("\n");	
	}
	if(V)printf("\n");

	if(V)printf("________DIAGONAL PRINCIPAL SAGITAL\n");
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+i,ijn(w-KERNEL+i,h-KERNEL+j,img_w));
		}
	if(V)	printf("\n");	
	}
	if(V)printf("\n");

	if(V)printf("______DIAGONAL SECUNDARIA SAGITAL\n");
	for(int i = PBASE-1; i >= 0; i--)
	{
		for(int j = 0; j < PBASE; j++)
		{
			if(V)printf("[%d,%d] ",d-KERNEL+i,ijn(w-KERNEL+i,h-KERNEL+j,img_w));
		}
		if(V)printf("\n");	
	}
	if(V)printf("\n");
}

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


	// printf("#########Img2D\n");
	// for (int i = 0; i < img1Info.resWidth; ++i)
	// {
	// 	for (int j = 0; j < img1Info.resHeight; ++j)
	// 	{
	// 		printf("%d ",data2[i*img1Info.resWidth+j]);			
	// 	}
	// 	printf("\n");
	// }
	// printf("\n\n\n");

	char *subImg;
	subImg = (char*)malloc(sizeof(char)* PBASE*PBASE);//sub imagens
	
	
	// for (int i = offset; i < img1Info.resWidth-offset; i++) //percorre imagem pixel //coluna
	// {
	// 	for (int j = offset; j < img1Info.resHeight-offset; j++) //a pixel //linha
	// 	{	
	// 		for(int ii=0; ii < pBase; ii++)
	// 		{
	// 			for(int jj=0; jj < pBase; jj++)
	// 			{
	// 				int pData2 = ijn(i-kernel+ii, j-kernel+jj ,img1Info.resWidth);
	// 				subImg[ijn(ii, jj, pBase)] = data2[pData2];
	// 				printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
	// 			}
	// 			printf("\n");
	// 		}
	// 		printf("\n\n\n");
	// 	}
	// }	

//****************************
	printf("############### PERCORRE VOLUME\n");



	clock_t start = clock();
	
	// // slice por slice comum
	// for (int k = img1Info.initStack; k < img1Info.endStack; ++k)
	// {
	// 	for (int i = 0; i < img1Info.resWidth; ++i)
	// 	{
	// 		for (int j = 0; j < img1Info.resHeight; ++j)
	// 		{
	// 			if(v)printf("[%d][%d] ",k,i*img1Info.resWidth+j);
	// 		}
	// 		if(v)printf("\n");
	// 	}
	// 	if(v)printf("\n\n");
	// }

	
	for (int d = OFFSET; d < img1Info.resDepth-OFFSET; d++)
	{
		for (int w = OFFSET; w < img1Info.resWidth-OFFSET; w++) //percorre imagem pixel //coluna
		{
			for (int h = OFFSET; h < img1Info.resHeight-OFFSET; h++) //a pixel //linha
			{	
				if(V)printf("\n[%d,%d]\n",d,ijn(w,h,img1Info.resWidth));
				buildImagePlanes(d,w,h,img1Info.resWidth);
				//if(V)printf("**** AXIAL\n");
				// for(int ii=0; ii < pBase; ii++)
				// {
				// 	for(int jj=0; jj < pBase; jj++)
				// 	{
				// 		int pData2 = ijn(i-kernel+ii, j-kernel+jj ,img1Info.resWidth);
				// 		//subImg[ijn(ii, jj, pBase)] = data1[k][pData2];
				// 		if(V) printf("[%d][%d] ", k, pData2 ); 
				// 		//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
				// 	}
				// 	if(V)printf("\n");
				// }
			}
		}
		if(V)printf("################\n");
	}

	start = clock() - start;
  	printf ("(%f seconds).\n",((float)start)/CLOCKS_PER_SEC);
	// printf("**** CORONAL \n");

	// std::swap(img1Info.resHeight,img1Info.resDepth);

	// // slice por slice coronal view
	// for (int k = offset; k < img1Info.resDepth-offset; ++k)
	// {
	// 	for (int i = offset; i < img1Info.resWidth-offset; ++i)
	// 	{
	// 		for (int j = offset; j < img1Info.resHeight-offset; ++j)
	// 		{
	// 			Xfor(int ii=0; ii < pBase; ii++)
	// 			{
	// 				for(int jj=0; jj < pBase; jj++)
	// 				{
	// 					int pData2 = ijn(i-kernel+ii, k-kernel+jj ,img1Info.resWidth);
	// 					subImg[ijn(ii, jj, pBase)] = data1[j][pData2];
	// 					printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
	// 				}
	// 				printf("\n");
	// 			}
	// 			printf("\n\n");
	// 		}
	// 	}
	// 	printf("################\n");	
	// }

	// printf("**** SAGITTAL \n");

	// // slice por slice sagittal view
	// for (int k = offset; k < img1Info.resDepth-offset; ++k)
	// {
	// 	for (int i = offset; i < img1Info.resWidth-offset; ++i)
	// 	{
	// 		for (int j = offset; j < img1Info.resHeight-offset; ++j)
	// 		{
	// 			for(int ii=0; ii < pBase; ii++)
	// 			{
	// 				for(int jj=0; jj < pBase; jj++)
	// 				{
	// 					int pData2 = ijn(k-kernel+ii, i-kernel+jj ,img1Info.resWidth);
	// 					subImg[ijn(ii, jj, pBase)] = data1[j][pData2];
	// 					printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
	// 				}
	// 				printf("\n");
	// 			}
	// 			printf("\n\n");
	// 		}
	// 	}
	// 	printf("################\n");	
	// }


	


	return 0;
}