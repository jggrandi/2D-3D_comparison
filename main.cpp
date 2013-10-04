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

typedef struct bestMatch
{
	float   bmValue;
	int     bmPlane;
	Point3i bmCoord; 
}BM;

void buidImagePlanes(int d, int w, int h, int resW, char **data1, int diag_type, Mat &t)
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

	Mat slice(PBASE,PBASE,CV_8SC1,subVol);
	t = slice.clone();
	slice.release();
	free(subVol);
	subVol=0;	
}

int main(int argc, char **argv)
{
	DATAINFO PP_RAW, PP_MOD;

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

	Handle3DDataset <char>d1(PP_RAW,PP_MOD);

	if(!d1.loadFile()){ printf("Fail to open: %s\n", PP_RAW.fileName ); return -1;}

	char **data1 = d1.getDataset(0);
	char  *data4 = d1.arbitraryPlane(10,'a',0);
	

	char **voxel = (char**)malloc(PP_RAW.resDepth * sizeof(char*));
	for (int i=0; i < PP_RAW.resDepth; i++)
		voxel[i] = (char*)malloc(sizeof(char) * (PP_RAW.resWidth*PP_RAW.resHeight));


	FILE *outFile;	
	if(!(outFile = fopen("image.raw", "wb+")))
		return false;
	//save the new view plane into a new raw file
		fwrite(data4, 1, sizeof(char*)*PP_RAW.resWidth*PP_RAW.resHeight, outFile);
	fclose(outFile);

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

	printf("Finding the best match... \n");
	clock_t start = clock();
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
			Mat sliceOrig(PBASE,PBASE,CV_8SC1,subImg);
		
			for (int vd = OFFSET; vd < PP_RAW.resDepth-OFFSET; vd++)
			{
				for (int vw = OFFSET; vw < PP_RAW.resWidth-OFFSET; vw++) //percorre imagem pixel //coluna
				{
					for (int vh = OFFSET; vh < PP_RAW.resHeight-OFFSET; vh++) //a pixel //linha
					{
						for (int p = 0; p < PLANES; p++)
						{
							Mat t;
							
							buidImagePlanes(vd,vw,vh,PP_RAW.resWidth,data1,p,t); //passa pro ref o t
	            			mpsnrV = qualAssess.getPSNR(sliceOrig,t);
							if(mpsnrV.val[0] == 0)
							{
								counts[p][0]++;
								counts[p][1]=vd-OFFSET;
								//std::cout << "SO = " << std::endl << " " << sliceOrig << std::endl << std::endl;							
								
								//std::cout << "T = "<<p << " ["<<iw <<","<<ih<<"]"<< " ["<<vd <<","<<vw<<","<<vh<<"]" << std::endl << " " << t << std::endl << std::endl;							
								
							}
							if(p == 0 && mpsnrV.val[0] != 0)
							{
							//	printf("%f\n", mpsnrV.val[0]*100 );
								voxel[vd][ijn(iw,ih,PP_RAW.resWidth)] = 255;
							}
							else if(p == 0 && mpsnrV.val[0] == 0)
							{
							//	printf("########%f\n", mpsnrV.val[0]);
								voxel[vd][ijn(iw,ih,PP_RAW.resWidth)] = 128;
							}
							//std::cout << "SO = "<<p << " ["<<vd <<","<<vw<<","<<vh<<"]"  << std::endl << " " << sliceOrig << std::endl << std::endl;							
						}
            			count3++;
					}
				}
			}
			count2++;
		}
	}

	DATAINFO saveVoxels;
	saveVoxels.fileName = "volumePlane.raw";
	saveVoxels.resWidth = PP_RAW.resWidth;
	saveVoxels.resHeight = PP_RAW.resHeight;
	saveVoxels.resDepth = PP_RAW.resDepth;
	if(d1.saveModifiedDataset(voxel, saveVoxels)) printf("Volume saved (%s)!\n", saveVoxels.fileName);

	int summ=0;
	for(int ix = 0; ix < PLANES; ix++)
	{
		summ = summ+counts[ix][0];
	}	
	free (subImg);
	start = clock() - start;
  	printf ("(%f seconds).\n",((float)start)/CLOCKS_PER_SEC);	
  	for(int i = 0; i < PLANES; i++)
  	{
  		printf("%d=>%d,%d\n",i,counts[i][0],counts[i][1]);
  	}
	printf("%d,%d,%d\n",count2,summ,count3 );


	return 0;		
}