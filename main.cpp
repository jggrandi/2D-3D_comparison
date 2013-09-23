#include <cstdlib>
#include <cstdio>

#include <handle3ddataset.h>
#include <qualityassessment.h>
#include <opencv2/core/core.hpp>  
#include <ctime>

using namespace cv;

#define ijn(a,b,n) ((a)*(n))+b
#define V false

#define KERNEL 4
#define PBASE KERNEL*2+1
#define OFFSET KERNEL


void buidImagePlanes(int d, int w, int h, int resW, char **data1, int diag_type, Mat &t)
{
	//char *subVol= new char[PBASE*PBASE];
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
	char *data2 = data1[KERNEL];

	char *subImg = (char*)malloc(sizeof(char*)* PBASE*PBASE);//sub imagens
	QualityAssessment qualAssess;
	Scalar mpsnrV;

	int count=0;
	int count2=0;
	int count3=0;
	clock_t start = clock();
	for (int iw = OFFSET; iw < img1Info.resDepth-OFFSET; iw++)
	{
		for (int ih = OFFSET; ih < img1Info.resWidth-OFFSET; ih++) //percorre imagem pixel //coluna
		{
			for(int ii=0; ii < PBASE; ii++)
			{
				for(int jj=0; jj < PBASE; jj++)
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
						Mat t;

						buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,0,t);
            				mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
						buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,1,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
            			buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,2,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
            			buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,3,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
            			buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,4,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
            			buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,5,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
            			buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,6,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
            			buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,7,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;         
						buidImagePlanes(vd,vw,vh,img1Info.resWidth,data1,8,t);
						mpsnrV = qualAssess.getPSNR(sliceOrig,t);
						if(mpsnrV.val[0] == 0) 	count++;
            			count3++;
					}
				}
			}
			count2++;
		}
		printf("%d\n",iw);
	}
	//free (subImg);
	start = clock() - start;
  	printf ("(%f seconds).\n",((float)start)/CLOCKS_PER_SEC);	
	printf("%d,%d,%d\n",count2,count,count3 );
	return 0;		
}