#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <handle3ddataset.h>
#include <qualityassessment.h>
#include <opencv2/core/core.hpp>  
#include <ctime>

using namespace cv;

#define ijn(a,b,n) ((a)*(n))+b
#define V false

#define KERNEL 1
#define PBASE  KERNEL*2+1
#define OFFSET KERNEL
#define PLANES 9

void buidImagePlanes(int d, int w, int h, int resW, char **data1, int diag_type, Mat &t)
{
	char *subVol = (char*)malloc(sizeof(char*)* PBASE*PBASE);//sub imagens
	int iC1,jC1,jC2;
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			switch(diag_type)
			{
				case 0:
					iC1 = d-KERNEL+1;
					jC1 = w-KERNEL+i;
					jC2 = h-KERNEL+j;
				break;
				case 1:
					iC1 = d-KERNEL+i;
					jC1 = w-KERNEL+j;
					jC2 = h-KERNEL+j;					
				break;
				case 2:
					iC1 = d-KERNEL+i;
					jC1 = w-KERNEL+j;
					jC2 = h-KERNEL-j-1;				
				break;
				case 3:
					iC1 = d-KERNEL+j;
					jC1 = w-KERNEL+1;
					jC2 = h-KERNEL+i;				
				break;
				case 4:
					iC1 = d-KERNEL+i;
					jC1 = w-KERNEL+j;
					jC2 = h-KERNEL+i;				
				break;
				case 5:
					iC1 = d-KERNEL+(PBASE-i-1);
					jC1 = w-KERNEL+j;
					jC2 = h-KERNEL+(PBASE-i-1);				
				break;
				case 6:
					iC1 = d-KERNEL+j;
					jC1 = w-KERNEL+i;
					jC2 = h-KERNEL+1;				
				break;
				case 7:
					iC1 = d-KERNEL+i;
					jC1 = w-KERNEL+i;
					jC2 = h-KERNEL+j;				
				break;
				case 8:
					iC1 = d-KERNEL+(PBASE-i-1);
					jC1 = w-KERNEL+(PBASE-i-1);
					jC2 = h-KERNEL+j;				
				break;			
				default:
				break;
			}		
			subVol[ijn(i,j,PBASE)] = data1[iC1][ijn(jC1,jC2,resW)];
		}			
	}		

	Mat slice(PBASE,PBASE,CV_8SC1,subVol);
	t = slice.clone();
	free(subVol);
	subVol=0;	
}

int main(int argc, char **argv)
{

	DATAINFO img1Info;
	DATAINFO img2Info;

	if (argc != 7)
	{
		printf("Falta argumentos");
		return -1;
	}

	img1Info.inputFileName= argv[1];
	img2Info.inputFileName= argv[2];

	img1Info.resWidth=img2Info.resWidth= atoi(argv[3]);
	img1Info.resHeight=img2Info.resHeight= atoi(argv[4]);
	img1Info.initStack=img2Info.initStack= atoi(argv[5]);
	img1Info.endStack=img2Info.endStack= atoi(argv[6]);
	img1Info.resDepth=img2Info.resDepth= img1Info.endStack - img1Info.initStack;
	//img1Info.viewOrientation = 's';

	printf("%s,%s:[%dx%dx%d]\n", img1Info.inputFileName,img2Info.inputFileName,img1Info.resWidth,img1Info.resHeight,img1Info.resDepth);
	
	Handle3DDataset <char>d1(img1Info);
	Handle3DDataset <char>d2(img2Info);

	if(!d1.loadFile()){ printf("Erro ao abrir: %s\n", img1Info.inputFileName ); return -1;}
	if(!d2.loadFile()){ printf("Erro ao abrir: %s\n", img2Info.inputFileName ); return -1;}

	char **data1 = d1.getDataset(0);
	char **data3 = d2.getDataset(0);
	//d1.changePlane();
	//char **data3 = d1.getDataset(1);
	char *data2 = data3[4];

	char *subImg = (char*)malloc(sizeof(char*)* PBASE*PBASE);//sub imagens
	QualityAssessment qualAssess;
	Scalar mpsnrV;

	int count2,count3;
	count2=count3=0;
	int counts[PLANES][2];

	for (int i = 0; i < PLANES; i++)
	{
		counts[i][0]=0;
		counts[i][1]=0;
	}

	clock_t start = clock();
	for (int iw = OFFSET; iw < img1Info.resDepth-OFFSET; iw++)
	{
		for (int ih = OFFSET; ih < img1Info.resWidth-OFFSET; ih++) //percorre imagem pixel //coluna
		{
			for(int ii = 0; ii < PBASE; ii++)
			{
				for(int jj = 0; jj < PBASE; jj++)
				{			
					subImg[ijn(ii,jj,PBASE)] = data2[ijn(iw-KERNEL+ii, ih-KERNEL+jj ,img1Info.resWidth)];
				}
			}
			Mat sliceOrig(PBASE,PBASE,CV_8SC1,subImg);
		
			for (int vd = OFFSET; vd < img1Info.resDepth-OFFSET; vd++)
			{
				for (int vw = OFFSET; vw < img1Info.resWidth-OFFSET; vw++) //percorre imagem pixel //coluna
				{
					for (int vh = OFFSET; vh < img1Info.resHeight-OFFSET; vh++) //a pixel //linha
					{
						//printf("[%d,%d]\n",d,ijn(w,h,IMG_W));
						for (int p = 0; p < PLANES; p++)
						{
							Mat t;
							buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,p,t);
	            			mpsnrV = qualAssess.getPSNR(sliceOrig,t);
							if(mpsnrV.val[0] == 0)
							{
								counts[p][0]++;
								counts[p][1]=vd-OFFSET;
							}
							//std::cout << "SO = " << std::endl << " " << sliceOrig << std::endl << std::endl;							
							std::cout << "T = "<<p << std::endl << " " << t << std::endl << std::endl;							
							

						}
            			count3++;
					}
				}
			}
			count2++;
		}
		printf("%d\n",iw);
	}
	int summ=0;
	for(int ix = 0; ix < PLANES; ix++)
	{
		summ = summ+counts[ix][0];
	}	
	//free (subImg);
	start = clock() - start;
  	printf ("(%f seconds).\n",((float)start)/CLOCKS_PER_SEC);	
  	for(int i = 0; i < PLANES; i++)
  	{
  		printf("%d=>%d,%d\n",i,counts[i][0],counts[i][1]);
  	}
	printf("%d,%d,%d\n",count2,summ,count3 );
	return 0;		
}