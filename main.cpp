//#include <cstdlib>
//#include <cstdio>

#include <handle3ddataset.h>
#include <qualityassessment_noOpenCV.h>  
#include <omp.h>
//#include <MutualInformation.h>

using namespace std;

#define ijn(a,b,n) ((a)*(n))+b

#define KERNEL 1
#define PBASE  KERNEL*2+1
#define OFFSET KERNEL
#define PLANES 9

typedef struct bestMatch
{
	char    bmSimValue;
	char 	bmColorValue;
	int     bmPlane;
	Point3i bmCoord; 
}BM;

//typedef unsigned short imgT;
typedef unsigned char imgT;

void buidImagePlanes(int d, int w, int h, int resW, imgT **data1, int diag_type, imgT *&t)
{
	int iC1,jC1,jC2;
	iC1=jC1=jC2=0;
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
			t[ijn(i,j,PBASE)] = data1[iC1][ijn(jC1,jC2,resW)];
		}			
	}
}


int main(int argc, char **argv)
{
	DATAINFO PP_RAW;

	if (argc < 6)
	{
		printf("Not enough parameters.");
		return -1;
	}

	PP_RAW.fileName = argv[1];
	PP_RAW.resWidth = atoi(argv[2]);
	PP_RAW.resHeight= atoi(argv[3]);
	PP_RAW.initStack= atoi(argv[4]);
	PP_RAW.endStack	= atoi(argv[5]);
	if(argv[6]!=NULL)
		PP_RAW.resampleFactor = atoi(argv[6]);
	else
		PP_RAW.resampleFactor = 1;

	PP_RAW.resDepth = PP_RAW.endStack - PP_RAW.initStack;
	

	printf("%s:[%dx%dx%d]:%d\n", PP_RAW.fileName,PP_RAW.resWidth,PP_RAW.resHeight,PP_RAW.resDepth,PP_RAW.resampleFactor);

	Handle3DDataset <imgT>d1(PP_RAW);

	if(!d1.loadFile()){ printf("Fail to open: %s\n", PP_RAW.fileName ); return -1;}

	imgT **data1 = d1.getDataset(0);
	imgT  *data4 = d1.arbitraryPlane();
	

	imgT **voxel = (imgT**)malloc(PP_RAW.resDepth * sizeof(imgT*));
	for (int i=0; i < PP_RAW.resDepth; i++)
		voxel[i] = (imgT*)malloc(sizeof(imgT) * (PP_RAW.resWidth*PP_RAW.resHeight));

	DATAINFO savePixels;
	savePixels.fileName = (char *) malloc(100);
	strcpy(savePixels.fileName,"imagePlane.raw");
	savePixels.resWidth = PP_RAW.resWidth;
	savePixels.resHeight = PP_RAW.resHeight;
	if(d1.saveModifiedImage(data4, savePixels)) printf("Image saved (%s)!\n", savePixels.fileName);

	
	//bestNow.bmSimValue = 1000;

	//BM bestMatches[PP_RAW.resDepth][PP_RAW.resWidth*PP_RAW.resHeight];


	BM **bestMatches = (BM**)calloc(PP_RAW.resDepth,PP_RAW.resDepth * sizeof(BM*));
	for (int i=0; i < PP_RAW.resDepth; i++)
		bestMatches[i] = (BM*)calloc(PP_RAW.resWidth*PP_RAW.resHeight, sizeof(BM) * (PP_RAW.resWidth*PP_RAW.resHeight));

	imgT *subImg = (imgT*)calloc(PBASE*PBASE,sizeof(imgT*)* PBASE*PBASE);//sub imagens

	int count2,count3;
	count2=count3=0;
	int counts[PLANES][2];

	for (int i = 0; i < PLANES; i++)
	{
		counts[i][0]=0;
		counts[i][1]=0;
	}
	
	printf("Finding the best match... \n");
	//double startTime, endTime;
	//startTime = getCPUTime( );
	double t1,t2;

	// double *x1, *x2;
	// x1 = new double[5];
	// x2 = new double[5];
	// x1[0]=1;x1[1]=1;x1[2]=1;x1[3]=0;x1[4]=0;
	// x2[0]=1;x2[1]=0;x2[2]=1;x2[3]=1;x2[4]=0;
	// double xxx = calculateMutualInformation(x1, x2, PBASE*PBASE);
	// printf("%f\n",xxx );

	
	t1=omp_get_wtime();
	
	int blackImage = 0;

   	FILE * pFile;
	pFile = fopen ("myfile.txt","w");

	for (int iw = OFFSET; iw < PP_RAW.resWidth-OFFSET; iw++)
	{
		printf(" %d\n",iw);
		for (int ih = OFFSET; ih < PP_RAW.resHeight-OFFSET; ih++) //percorre imagem pixel //coluna
		{
			blackImage=0;
			for(int ii = 0; ii < PBASE; ii++)
			{
				for(int jj = 0; jj < PBASE; jj++)
				{			
					subImg[ijn(ii,jj,PBASE)] = data4[ijn(iw-KERNEL+ii, ih-KERNEL+jj ,PP_RAW.resWidth)];
					if(subImg[ijn(ii,jj,PBASE)] <= 10)
						blackImage++;
				}
			}
				
			if(blackImage<PBASE*PBASE)		
			{

				#pragma omp parallel for 

				for (int vd = OFFSET; vd < PP_RAW.resDepth-OFFSET; vd++)
				{
					QualityAssessment qualAssess;
					Scalar mpsnrV;					
					imgT *t = (imgT*)calloc(PBASE*PBASE,sizeof(imgT*)* PBASE*PBASE);//sub imagens
					BM bestNow;
					for (int vw = OFFSET; vw < PP_RAW.resWidth-OFFSET; vw+=PP_RAW.resampleFactor) //percorre imagem pixel //coluna
					{
						for (int vh = OFFSET; vh < PP_RAW.resHeight-OFFSET; vh+=PP_RAW.resampleFactor /*vh+=4*/) //a pixel //linha
						{

							float bN = 1000;
							bool grava=false;
							int sameVoxel = 0;
							for (int p = 0; p < PLANES; p++)
							{
								buidImagePlanes(vd,vw,vh,PP_RAW.resWidth,data1,p,t); //passa pro ref o t
		            			mpsnrV = qualAssess.getPSNR<imgT>(subImg,t,PBASE,PBASE,PBASE);

		            			//printf("%f\n", mpsnrV.val[0] );
								if(mpsnrV.val[0] <= bN)
								{
									if(mpsnrV.val[0]==0)
									{	
								//		printf("+++++++++++++++++++\n");
								//		scanf("%d",&blackImage);
										bestNow.bmSimValue = 255;
										grava=true;
										counts[p][0]++;
										counts[p][1]=vd-OFFSET;
										sameVoxel++;
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
							// if(sameVoxel != 0)
							// 	printf("%d\n", sameVoxel);
							if((grava == true) && (sameVoxel == 1))
							{
								bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)] = bestNow;
								float bX = ((float)bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.x / (float)PP_RAW.resWidth * 2.0f) - 1.0f;
								float bY = ((float)bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.y / (float)PP_RAW.resHeight* 2.0f) - 1.0f;
								float bZ = ((float)bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.z / (float)PP_RAW.resDepth * 2.0f) - 1.0f;								
								printf("%f %f %f\n", bX, bY, bZ );
								fprintf(pFile, "%f %f %f\n", bX, bY, bZ );
							}
							bN = 1000;
	            			count3++;
						}

					}
				}
			}
			count2++;
		}
	}

	//endTime = getCPUTime( );
	//fprintf( stderr, "CPU time used = %lf\n", (endTime - startTime) );

	t2=omp_get_wtime();
	printf("Time with stack array: %12.3f sec, \n", t2-t1);

	imgT **simVolume = (imgT**)calloc(PP_RAW.resWidth, PP_RAW.resDepth * sizeof(imgT*));

	fclose(pFile);
	//imgT simVolume[PP_RAW.resDepth][PP_RAW.resWidth*PP_RAW.resHeight];

	for (int i=0; i < PP_RAW.resDepth; i++)
		simVolume[i] = (imgT*)calloc(PP_RAW.resWidth, sizeof(imgT) * (PP_RAW.resWidth*PP_RAW.resHeight));

	imgT *simImg = (imgT*)calloc(PP_RAW.resWidth , sizeof(imgT) * (PP_RAW.resWidth*PP_RAW.resHeight));

	for (int d = 0; d < PP_RAW.resDepth; d++)
	{
		for (int w = 0; w < PP_RAW.resWidth; w++)
		{
			for (int h = 0; h < PP_RAW.resHeight; h++)
			{
				simVolume[d][ijn(w,h,PP_RAW.resWidth)] = (imgT)bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue;
				// if(bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue == 0)
				// 	printf("%d=>%f\t%d *********\n",bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmPlane,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmColorValue  );
				// else
				// 	printf("%d=>%f\t%d\n",bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmPlane,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue,bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmColorValue  );
			}

		}
	}

 //   FILE * pFile;
	// pFile = fopen ("myfile.txt","w");

	// for (int d = 0; d < PP_RAW.resDepth; d++)
	// {
	// 	for (int w = 0; w < PP_RAW.resWidth; w++)
	// 	{
	// 		for (int h = 0; h < PP_RAW.resHeight; h++)
	// 		{
	// 			if(bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmSimValue == 0)
	// 			{
	// 				fprintf(pFile, "%d %d %d\n",bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmCoord.x, bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmCoord.y, bestMatches[d][ijn(w,h,PP_RAW.resWidth)].bmCoord.z );
	// 			}
	// 		}

	// 	}
	// }
	// fclose (pFile);

	DATAINFO saveVoxels;
	saveVoxels.fileName = (char *) malloc(100);
	strcpy(saveVoxels.fileName,"volumePlane.raw");
	saveVoxels.resWidth = PP_RAW.resWidth;
	saveVoxels.resHeight = PP_RAW.resHeight;
	saveVoxels.resDepth = PP_RAW.resDepth;
	if(d1.saveModifiedDataset<imgT>(simVolume, saveVoxels)) printf("Volume saved (%s)!\n", saveVoxels.fileName);

	int summ=0;
	for(int ix = 0; ix < PLANES; ix++)
	{
		summ = summ+counts[ix][0];
	}	
	free (subImg);
	

  	
  	for(int i = 0; i < PLANES; i++)
  	{
  		printf("%d=>%d,%d\n",i,counts[i][0],counts[i][1]);
  	}
	printf("%d,%d,%d\n",count2,summ,count3 );


	return 0;		
}
