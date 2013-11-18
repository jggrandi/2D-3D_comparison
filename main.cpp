#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <handle3ddataset.h>
#include <qualityassessment.h>
#include <opencv2/core/core.hpp>  
#include <cputime.h>

using namespace cv;

#define ijn(a,b,n) ((a)*(n))+b
#define V false

#define KERNEL 1
#define PBASE  KERNEL*2+1
#define OFFSET KERNEL
#define PLANES 9

typedef struct bestMatch
{
	char   bmSimValue;
	char 	bmColorValue;
	int     bmPlane;
	Point3i bmCoord; 
}BM;

void buidImagePlanes(int d, int w, int h, int resW, char **data1, int diag_type, char *&t)
{
	char *subVol=(char*)malloc(sizeof(char*)* PBASE*PBASE);//sub imagens
	int iC1,jC1,jC2;
	int dK = d-KERNEL;
	int wK = w-KERNEL;
	int hK = h-KERNEL;
	for(int i = 0; i < PBASE; i++)
	{
		for(int j = 0; j < PBASE; j++)
		{
			switch(diag_type)
			{
				case 0:
					iC1 = dK+1;
					jC1 = wK+i;
					jC2 = hK+j;
				break;
				case 1:
					iC1 = dK+i;
					jC1 = wK+j;
					jC2 = hK+j;					
				break;
				case 2:
					iC1 = dK+i;
					jC1 = wK+j;
					jC2 = hK+(PBASE-j-1);				
				break;
				case 3:
					iC1 = dK+j;
					jC1 = wK+1;
					jC2 = hK+i;				
				break;
				case 4:
					iC1 = dK+i;
					jC1 = wK+j;
					jC2 = hK+i;				
				break;
				case 5:
					iC1 = dK+(PBASE-i-1);
					jC1 = wK+j;
					jC2 = hK+(PBASE-i-1);				
				break;
				case 6:
					iC1 = dK+j;
					jC1 = wK+i;
					jC2 = hK+1;				
				break;
				case 7:
					iC1 = dK+i;
					jC1 = wK+i;
					jC2 = hK+j;				
				break;
				case 8:
					iC1 = dK+(PBASE-i-1);
					jC1 = wK+(PBASE-i-1);
					jC2 = hK+j;				
				break;			
				default:
				break;
			}		
			subVol[ijn(i,j,PBASE)] = data1[iC1][ijn(jC1,jC2,resW)];
		}			
	}
	t=subVol;		
	//Mat slice(PBASE,PBASE,CV_8SC1,subVol);
	//t = slice.clone();
	//slice.release();
	free(subVol);
	subVol=0;	
	//return subVol;
}

int main(int argc, char **argv)
{
	DATAINFO PP_RAW;

	if (argc != 6)
	{
		printf("Not enough parameters.");
		return -1;
	}

	PP_RAW.fileName = argv[1];
	PP_RAW.resWidth = atoi(argv[2]);
	PP_RAW.resHeight= atoi(argv[3]);
	PP_RAW.initStack= atoi(argv[4]);
	PP_RAW.endStack	= atoi(argv[5]);
	PP_RAW.resDepth = PP_RAW.endStack - PP_RAW.initStack;
	
	printf("%s:[%dx%dx%d]\n", PP_RAW.fileName,PP_RAW.resWidth,PP_RAW.resHeight,PP_RAW.resDepth);

	Handle3DDataset <char>d1(PP_RAW);

	if(!d1.loadFile()){ printf("Fail to open: %s\n", PP_RAW.fileName ); return -1;}

	char **data1 = d1.getDataset(0);
	char  *data4 = d1.arbitraryPlane(4,'a',0);
	

	char **voxel = (char**)malloc(PP_RAW.resDepth * sizeof(char*));
	for (int i=0; i < PP_RAW.resDepth; i++)
		voxel[i] = (char*)malloc(sizeof(char) * (PP_RAW.resWidth*PP_RAW.resHeight));

	DATAINFO savePixels;
	savePixels.fileName = "imagePlane.raw";
	savePixels.resWidth = PP_RAW.resWidth;
	savePixels.resHeight = PP_RAW.resHeight;
	if(d1.saveModifiedImage(data4, savePixels)) printf("Image saved (%s)!\n", savePixels.fileName);
	
	BM bestNow;
	//bestNow.bmSimValue = 1000;
	BM bestMatches[PP_RAW.resDepth][PP_RAW.resWidth*PP_RAW.resHeight];

	// BM **bestMatches = (BM**)malloc(PP_RAW.resDepth * sizeof(BM*));
	// for (int i=0; i < PP_RAW.resDepth; i++)
	// 	bestMatches[i] = (BM*)malloc(sizeof(BM) * (PP_RAW.resWidth*PP_RAW.resHeight));

	char *subImg = (char*)malloc(sizeof(char*)* PBASE*PBASE);//sub imagens
	QualityAssessment qualAssess;
	Scalar mpsnrV;

	char *t = (char*)malloc(sizeof(char*)* PBASE*PBASE);//sub imagens

	int count2,count3;
	count2=count3=0;
	int counts[PLANES][2];

	for (int i = 0; i < PLANES; i++)
	{
		counts[i][0]=0;
		counts[i][1]=0;
	}

	printf("Finding the best match... \n");
	double startTime, endTime;

	startTime = getCPUTime( );

	for (int iw = OFFSET; iw < PP_RAW.resWidth-OFFSET; iw++)
	{
		printf(" %d\n",iw);
		for (int ih = OFFSET; ih < PP_RAW.resHeight-OFFSET; ih++) //percorre imagem pixel //coluna
		{
			for(int ii = 0; ii < PBASE; ii++)
			{
				for(int jj = 0; jj < PBASE; jj++)
				{			
					subImg[ijn(ii,jj,PBASE)] = data4[ijn(iw-KERNEL+ii, ih-KERNEL+jj ,PP_RAW.resWidth)];
				}
			}
			//Mat sliceOrig(PBASE,PBASE,CV_8SC1,subImg);
		
			for (int vd = OFFSET; vd < PP_RAW.resDepth-OFFSET; vd++)
			{
				for (int vw = OFFSET; vw < PP_RAW.resWidth-OFFSET; vw++) //percorre imagem pixel //coluna
				{
					for (int vh = OFFSET; vh < PP_RAW.resHeight-OFFSET; vh++) //a pixel //linha
					{
						float bN = 1000;
						bool grava=false;
						for (int p = 0; p < PLANES; p++)
						{
							//Mat t;
							
							buidImagePlanes(vd,vw,vh,PP_RAW.resWidth,data1,p,t); //passa pro ref o t
	            			mpsnrV = qualAssess.getPSNR(subImg,t,PBASE,PBASE,PBASE);
							printf("%f(%d,%d,%d)\n",mpsnrV.val[0],vd,vw,vh );
							// if(mpsnrV.val[0] < 1)
							// {
							// 	printf("[%dx%d]=%d,%f\t*****\n",iw,ih,p, mpsnrV.val[0] ); 							
							// 	int a;
							// 	scanf("%d",&a);
							// }
							// printf("[%dx%d]=%d,%f\n",iw,ih,p, mpsnrV.val[0] );
	            			//printf("%d,%f\n",p,bestNow.bmSimValue );
							if(mpsnrV.val[0] <= bN)
							{
								if(mpsnrV.val[0]==0)
								{	
									printf("A\n");
									bestNow.bmSimValue = 255;
									grava=true;
									counts[p][0]++;
									counts[p][1]=vd-OFFSET;
								}
								else
									grava=false;
								bestNow.bmColorValue = data1[vd][ijn(vw,vh,PP_RAW.resWidth)];			
								bestNow.bmPlane = p;
								bestNow.bmCoord.x = vd;
								bestNow.bmCoord.y = vw;
								bestNow.bmCoord.z = vh;

								bN = mpsnrV.val[0];

							}
						}
						if(grava)
							bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)] = bestNow;
						bN = 1000;
            			count3++;
					}
				}
			}
			count2++;
		}
	}

	char **simVolume = (char**)calloc(PP_RAW.resWidth, PP_RAW.resDepth * sizeof(char*));
	for (int i=0; i < PP_RAW.resDepth; i++)
		simVolume[i] = (char*)calloc(PP_RAW.resWidth , sizeof(char) * (PP_RAW.resWidth*PP_RAW.resHeight));

	for (int d = OFFSET; d < PP_RAW.resDepth-OFFSET; d++)
	{
		for (int w = OFFSET; w < PP_RAW.resWidth-OFFSET; w++)
		{
			for (int h = OFFSET; h < PP_RAW.resHeight-OFFSET; h++)
			{
				simVolume[d][ijn(w,h,PP_RAW.resWidth)] = (char)bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmColorValue;
				// if(bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue == 0)
				// 	printf("%d=>%f\t%d *********\n",bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmPlane,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmColorValue  );
				// else
				// 	printf("%d=>%f\t%d\n",bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmPlane,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmColorValue  );
			}

		}
	}


	DATAINFO saveVoxels;
	saveVoxels.fileName = "volumePlane.raw";
	saveVoxels.resWidth = PP_RAW.resWidth;
	saveVoxels.resHeight = PP_RAW.resHeight;
	saveVoxels.resDepth = PP_RAW.resDepth;
	if(d1.saveModifiedDataset<char>(simVolume, saveVoxels)) printf("Volume saved (%s)!\n", saveVoxels.fileName);

	int summ=0;
	for(int ix = 0; ix < PLANES; ix++)
	{
		summ = summ+counts[ix][0];
	}	
	free (subImg);
	
	endTime = getCPUTime( );
	fprintf( stderr, "CPU time used = %lf\n", (endTime - startTime) );


  	for(int i = 0; i < PLANES; i++)
  	{
  		printf("%d=>%d,%d\n",i,counts[i][0],counts[i][1]);
  	}
	printf("%d,%d,%d\n",count2,summ,count3 );


	return 0;		
}