#include <cstdlib>
#include <handle3ddataset.h>
#include <fstream>
#include <ctime>

#define ijn(a,b,n) ((a)*(n))+b

int main(int argc, char **argv)
{
	int option_index = 0;
	bool v = false; // verbose mode
	bool approx_runtime = false;
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
	
	int kernel = 1; //tamanho do kernel
	int pBase = (kernel * 2) + 1;
	int offset = kernel; // o offset minimo é o tamanho do kernel
	subImg = (char*)malloc(sizeof(char)* pBase*pBase);//sub imagens
	
	
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


	clock_t start = clock();
	clock_t time_estimated;

	for (int iii = offset; iii < img1Info.resWidth-offset; iii++) //percorre imagem pixel //coluna
	{
		for (int jjj = offset; jjj < img1Info.resHeight-offset; jjj++) //a pixel //linha
		{	
			for(int iiii=0; iiii < pBase; iiii++)
			{
				for(int jjjj=0; jjjj < pBase; jjjj++)
				{

					if(jjj==8 && !approx_runtime) time_estimated = clock();
					for (int k = offset; k < img1Info.resDepth-offset; k++)
					{
						for (int i = offset; i < img1Info.resWidth-offset; i++) //percorre imagem pixel //coluna
						{
							for (int j = offset; j < img1Info.resHeight-offset; j++) //a pixel //linha
							{	
								if(v)printf("%d\n",data1[k][ijn(i,j,img1Info.resWidth)]);
								if(v)printf("**** AXIAL\n");
								for(int ii=0; ii < pBase; ii++)
								{
									for(int jj=0; jj < pBase; jj++)
									{
										int pData2 = ijn(i-kernel+ii, j-kernel+jj ,img1Info.resWidth);
										subImg[ijn(ii, jj, pBase)] = data1[k][pData2];
										if(v) printf("[%d][%d] ", k, pData2 ); 
										//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
									}
									if(v)printf("\n");
								}
								//subImg = NULL;
								//subImg = (char*)malloc(sizeof(char)* pBase*pBase);
								if(v)printf("**** CORONAL\n");
								for(int ii=0; ii < pBase; ii++)
								{
									for(int jj=0; jj < pBase; jj++)
									{
										int pData2 = ijn(i-kernel+ii, j-kernel+jj ,img1Info.resWidth);
										subImg[ijn(ii, jj, pBase)] = data1[k-kernel+ii][pData2];
										if(v)printf("[%d][%d] ", k-kernel+ii, pData2 ); 
										//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
									}
									if(v)printf("\n");
								}
								//subImg = NULL;
								//subImg = (char*)malloc(sizeof(char)* pBase*pBase);				
								if(v)printf("**** SAGITTAL\n");
								for(int ii=0; ii < pBase; ii++)
								{
									for(int jj=0; jj < pBase; jj++)
									{
										int pData2 = ijn(k-kernel+ii, i-kernel+jj ,img1Info.resWidth);
										subImg[ijn(ii, jj, pBase)] = data1[j-kernel+jj][pData2];
										if(v)printf("[%d][%d] ", j-kernel+jj, pData2 ); 
										//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
									}
									if(v)printf("\n");
								}
								//subImg = NULL;
								//subImg = (char*)malloc(sizeof(char)* pBase*pBase);
								if(v)printf("**** TRANSVERSARL 1\n");
								for(int ii=0; ii < pBase; ii++)
								{
									for(int jj=0; jj < pBase; jj++)
									{
										int pData2 = ijn(i-kernel+ii, j-kernel+jj ,img1Info.resWidth);
										subImg[ijn(ii, jj, pBase)] = data1[k-kernel+ii][pData2];
										if(v)printf("[%d][%d] ", k-kernel+ii, pData2 ); 
										//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
									}
									if(v)printf("\n");
								}
								//subImg = NULL;
								//subImg = (char*)malloc(sizeof(char)* pBase*pBase);				
								if(v)printf("**** TRANSVERSAL 2\n");
								for(int ii=0; ii < pBase; ii++)
								{
									for(int jj=0; jj < pBase; jj++)
									{
										int pData2 = ijn(k-kernel+ii, i-kernel+jj ,img1Info.resWidth);
										subImg[ijn(ii, jj, pBase)] = data1[j-kernel+jj][pData2];
										if(v)printf("[%d][%d] ", j-kernel+jj, pData2 ); 
										//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
									}
									if(v)printf("\n");
								}
								//subImg = NULL;
								//subImg = (char*)malloc(sizeof(char)* pBase*pBase);
								if(v)printf("**** TRANSVERSAL 3\n");
								for(int ii=0; ii < pBase; ii++)
								{
									for(int jj=0; jj < pBase; jj++)
									{
										int pData2 = ijn(i-kernel+jj, j-kernel+jj ,img1Info.resWidth);
										subImg[ijn(ii, jj, pBase)] = data1[k-kernel+ii][pData2];
										if(v)printf("[%d][%d] ", k-kernel+ii, pData2 ); 
										//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
									}
									if(v)printf("\n");
								}
								//subImg = NULL;
								//subImg = (char*)malloc(sizeof(char)* pBase*pBase);
								if(v)printf("**** TRANSVERSAL 4\n");
								for(int ii=0; ii < pBase; ii++)
								{
									for(int jj=0; jj < pBase; jj++)
									{
										int pData2 = ijn(i-kernel+ii, j-kernel+ii ,img1Info.resWidth);
										subImg[ijn(ii, jj, pBase)] = data1[k-kernel+ii][pData2];
										if(v)printf("[%d][%d] ", k-kernel+jj, pData2 ); 
										//printf("%d ", (int)subImg[ijn(ii, jj, pBase)] ); 
									}
									if(v)printf("\n");
								}								

							}
						}
						if(v)printf("################\n");
					}
					if(jjj==8 && !approx_runtime) 
					{
						time_estimated = clock() - time_estimated;
  						printf ("(Approx runtime:%f seconds).\n",img1Info.resWidth*img1Info.resWidth*pBase*pBase*((float)time_estimated)/CLOCKS_PER_SEC);
						printf ("(One interaction: %f seconds).\n",((float)time_estimated)/CLOCKS_PER_SEC);  						
  						approx_runtime = true;
  					}
				}
			}
		}
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
	// 			for(int ii=0; ii < pBase; ii++)
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