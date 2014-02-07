// #include <cstdlib>
// #include <cstdio>
// #include <cstring>
// #include <sstream>
#include <handle3ddataset.h>
#include <qualityassessment_noOpenCV.h>  
#include <omp.h>
#include <vector>
#include <iqa.h>
#include <cmath>
#include <fstream>
//#include <MutualInformation.h>
//#include <Entropy.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/linear_least_squares_fitting_3.h>

#include "vector3f.h"
using namespace std;

#define ijn(a,b,n) ((a)*(n))+b

#define KERNEL 1
#define PBASE  KERNEL*2+1
#define OFFSET KERNEL
#define PLANES 9
#define PSWEEP 32

static const struct iqa_ssim_args ssim_args = {
    0.39f,      /* alpha */
    0.731f,     /* beta */
    1.12f,      /* gamma */
    187,        /* L */
    0.025987f,  /* K1 */
    0.0173f,    /* K2 */
    1           /* factor */
};

typedef struct point3int
{
	int x;
	int y;
	int z;
}P3i;


typedef struct bestMatch
{
	char    bmSimValue;
	char 	bmColorValue;
	int     bmPlane;
	P3i 	bmCoord; 
}BM;

//typedef unsigned short imgT;
typedef unsigned char imgT;

typedef double                      FT;
typedef CGAL::Simple_cartesian<FT>  K;
typedef K::Line_3                   Line;
typedef K::Plane_3                  Plane;
typedef K::Point_3                  Point;


void buidImagePlanes(int d, int w, int h, int resW, imgT **&data1, int diag_type, imgT *&t)
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

	if (argc < 9)
	{
		printf("Not enough parameters.\n");
		return -1;
	}

	PP_RAW.fileName = argv[1];
	PP_RAW.resWidth = atoi(argv[2]);
	PP_RAW.resHeight= atoi(argv[3]);
	PP_RAW.initStack= atoi(argv[4]);
	PP_RAW.endStack	= atoi(argv[5]);
	
	PP_RAW.resampleFactorImg = atoi(argv[6]);	
	PP_RAW.resampleFactor 	 = atoi(argv[7]);	
	PP_RAW.resampleFactorZ   = atoi(argv[8]);

	PP_RAW.resDepth = PP_RAW.endStack - PP_RAW.initStack;
	
	printf("%s:[%dx%dx%d]:Rimg[%d]:Rvol[%d,%d]\n", PP_RAW.fileName,PP_RAW.resWidth,PP_RAW.resHeight,PP_RAW.resDepth,PP_RAW.resampleFactorImg, PP_RAW.resampleFactor, PP_RAW.resampleFactorZ);

	Handle3DDataset <imgT>d1(PP_RAW);
	
	if(!d1.loadFile()){ printf("Fail to open: %s\n", PP_RAW.fileName ); return -1;}

	vector3f vec_normal;
	float plane_d;

	float interp1= 30000;
	float interp2= 30000;

	imgT **data1 = d1.getDataset(0);
	ofstream ofs;


	stringstream output;
	output << PP_RAW.fileName << "_" <<PP_RAW.resampleFactorImg << "_"<<PP_RAW.resampleFactor << "_"<<PP_RAW.resampleFactorZ << ".csv";
	string sulfix = output.str();
	const char* ss = sulfix.c_str();

	//printf("%s\n",ss );
	ofs.open(ss);

	int planeSweep = (PP_RAW.resWidth)/PSWEEP;
	int incInterp = 0;
	for(int t=0; t<PP_RAW.resWidth; t+=planeSweep,incInterp++)
	{
		printf("%d\n",t );		
		interp1 = interp1 /incInterp;
		//interp2 = interp2 /t;

		imgT *data4 = (imgT*)calloc(PP_RAW.resWidth*PP_RAW.resHeight,sizeof(imgT*)* PP_RAW.resWidth*PP_RAW.resHeight);//sub imagens

		d1.arbitraryPlane(data4,PP_RAW.resWidth/2,t,interp1,interp2,vec_normal,plane_d);


		DATAINFO savePixels;
		savePixels.fileName = (char *) malloc(100);
		strcpy(savePixels.fileName,"imagePlane.raw");
		savePixels.resWidth = PP_RAW.resWidth;
		savePixels.resHeight = PP_RAW.resHeight;
		/*if(*/d1.saveModifiedImage(data4, savePixels);//) printf("Image saved (%s)!\n", savePixels.fileName);
		free(savePixels.fileName);
		savePixels.fileName=NULL;
		//bestNow.bmSimValue = 1000;

		//BM bestMatches[PP_RAW.resDepth][PP_RAW.resWidth*PP_RAW.resHeight];



		
		//printf("Finding the best match... \n");
		//double startTime, endTime;
		//startTime = getCPUTime( );
		double t1,t2;

		// double *x1, *x2;
		// x1 = new double[5];
		// x2 = new double[5];
		// x1[0]=1.0f;x1[1]=1.0f;x1[2]=1.0f;x1[3]=0;x1[4]=0;
		// x2[0]=1.0f;x2[1]=0;x2[2]=1.0f;x2[3]=1.0f;x2[4]=0;
		// double xxx = calculateMutualInformation(x1, x2, 5);
		// double yyy = calculateEntropy(x1,5);
		// printf("%f, %f\n",xxx,yyy );

		BM **bestMatches = (BM**)calloc(PP_RAW.resDepth,PP_RAW.resDepth * sizeof(BM*));
		for (int i=0; i < PP_RAW.resDepth; i++)
			bestMatches[i] = (BM*)calloc(PP_RAW.resWidth*PP_RAW.resHeight, sizeof(BM) * (PP_RAW.resWidth*PP_RAW.resHeight));

		vector<Point> bestCoords;



		int count2,count3;
		count2=count3=0;
		int counts[PLANES][2];

		for (int i = 0; i < PLANES; i++)
		{
			counts[i][0]=0;
			counts[i][1]=0;
		}

		imgT *subImg = (imgT*)calloc(PBASE*PBASE,sizeof(imgT*)* PBASE*PBASE);//sub imagens
		t1=omp_get_wtime();
		
		int planeDirection[9]={0,0,0,0,0,0,0,0};
		
		for (int iw = OFFSET; iw < PP_RAW.resWidth-OFFSET; iw+=PP_RAW.resampleFactorImg)
		{
			int blackImage = 0;

			bool correctMatch = false;		
			//printf(" %d\n",iw);
			for (int ih = OFFSET; ih < PP_RAW.resHeight-OFFSET; ih+=PP_RAW.resampleFactorImg) //percorre imagem pixel //coluna
			{
				//if(correctMatch==0)
				//{	
					//printf("%d\n",correctMatch );
					blackImage=0;
					for(int ii = 0; ii < PBASE; ii++)
					{
						for(int jj = 0; jj < PBASE; jj++)
						{			
							subImg[ijn(ii,jj,PBASE)] = data4[ijn(iw-KERNEL+ii, ih-KERNEL+jj ,PP_RAW.resWidth)];
							if(subImg[ijn(ii,jj,PBASE)] <= 20)
								blackImage++;
						}
					}
						
					if(blackImage<PBASE*PBASE)		
					{


						#pragma omp parallel for
						 	

						for (int vd = OFFSET; vd < PP_RAW.resDepth-OFFSET; vd+=PP_RAW.resampleFactorZ /*=PP_RAW.resampleFactor*/)
						{
							
							bool allow = true;
							QualityAssessment qualAssess;
							float mpsnrV;					
							imgT *t = (imgT*)calloc(PBASE*PBASE,sizeof(imgT*)* PBASE*PBASE);//sub imagens
							BM bestNow;
							for (int vw = OFFSET; vw < PP_RAW.resWidth-OFFSET; vw+=PP_RAW.resampleFactor) //percorre imagem pixel //coluna
							{
								if(allow)
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
					            		
					            			//mpsnrV = qualAssess.getMSE<imgT>(subImg,t,PBASE,PBASE,PBASE);
					            			//mpsnrV = iqa_ssim(t,subImg,PBASE,PBASE,PBASE,1,&ssim_args);
					            			//mpsnrV = iqa_ms_ssim(t,subImg,PBASE,PBASE,PBASE,0);
					            			//mpsnrV = iqa_psnr(subImg,t,PBASE,PBASE,PBASE);
					            			//mpsnrV = iqa_mse(subImg,t,PBASE,PBASE,PBASE);
					            			//mpsnrV = calculateMutualInformation(sb, st, PBASE*PBASE);
					            			//float iqa_ssim(const unsigned char *ref, const unsigned char *cmp, int w, int h, int stride, int gaussian, const struct iqa_ssim_args *args);
					            			// if(mpsnrV <= 3.0f)
					            			// 	printf("%f\n", mpsnrV );
											if(mpsnrV <= bN)
											{
												if(mpsnrV<=0.0f)
												{	
											//		printf("%f\n", mpsnrV);
											//		printf("+++++++++++++++++++\n");
											//		scanf("%d",&blackImage);
													bestNow.bmSimValue = 255;
													grava=true;
													counts[p][0]++;
													counts[p][1]=vd-OFFSET;
													sameVoxel++;
													bestNow.bmColorValue = data1[vd][ijn(vw,vh,PP_RAW.resWidth)];			
													bestNow.bmPlane = p;
													bestNow.bmCoord.x = vd;
													bestNow.bmCoord.y = vw;
													bestNow.bmCoord.z = vh;
													correctMatch++;
													if(sameVoxel==1)
														allow = false;
													planeDirection[p]++;
													// for (int xd=0; xd<9;xd++)
													// {
													// 	printf("[%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",planeDirection[0],planeDirection[1],planeDirection[2],planeDirection[3],planeDirection[4],planeDirection[5],planeDirection[6],planeDirection[7],planeDirection[8] );
													// }

												}
												else
													grava=false;


												bN = mpsnrV;

											}	
										}
										int greaterDirection = 0;

										//sort o vetor para saber qual direção tem a maior similaridade
										for(int ix=0; ix<8; ix++)
											if(planeDirection[ix+1] > planeDirection[greaterDirection])
												greaterDirection = ix+1;
						

										// printf("%d\n", greaterDirection);

										// Aqui são salvos as posições dos voxels com maior similaridade. Antes as coordenadas são condicionadas de -1 a 1 para posterior visualização das informações
										if((grava == true) && (sameVoxel == 1))
										{
											if(bestNow.bmPlane == greaterDirection )
											{
												bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)] = bestNow;
												Point coord(((float)bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.x / (float)PP_RAW.resWidth * 2.0f) - 1.0f,((float)bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.y / (float)PP_RAW.resHeight* 2.0f) - 1.0f,((float)bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.z / (float)PP_RAW.resDepth * 2.0f) - 1.0f);
												//Point coord(bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.x,bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.y,bestMatches[vd][ijn(vw,vh,PP_RAW.resWidth)].bmCoord.z);
												bestCoords.push_back(coord);
											}
											//printf("%f %f %f\n", coord.x, coord.y, coord.z );
						
										}
										bN = 1000;
				            			count3++;
									}
								}
							}
							free(t);
							t=NULL;
						}
					}
				//}
				count2++;
			}
			correctMatch=0;
		}
		//endTime = getCPUTime( );
		//fprintf( stderr, "CPU time used = %lf\n", (endTime - startTime) );
		free (subImg);
		subImg=NULL;

		t2=omp_get_wtime();
		printf("Time with stack array: %12.3f sec, \n", t2-t1);

		imgT **simVolume = (imgT**)calloc(PP_RAW.resWidth, PP_RAW.resDepth * sizeof(imgT*));
		for (int i=0; i < PP_RAW.resDepth; i++)
			simVolume[i] = (imgT*)calloc(PP_RAW.resWidth, sizeof(imgT) * (PP_RAW.resWidth*PP_RAW.resHeight));


		//excluir os pontos que não são do plano principal
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


		DATAINFO saveVoxels;
		saveVoxels.fileName = (char *) malloc(100);
		strcpy(saveVoxels.fileName,"volumePlane.raw");
		saveVoxels.resWidth = PP_RAW.resWidth;
		saveVoxels.resHeight = PP_RAW.resHeight;
		saveVoxels.resDepth = PP_RAW.resDepth;
		/*if(*/d1.saveModifiedDataset<imgT>(simVolume, saveVoxels);/*) printf("Volume saved (%s)!\n", saveVoxels.fileName);*/
		free(saveVoxels.fileName);
		saveVoxels.fileName =NULL;

		int summ=0;
		for(int ix = 0; ix < PLANES; ix++)
		{
			summ = summ+counts[ix][0];
		}	

		//printf("%f,%f,%f\n",vec_normal.z,vec_normal.y,vec_normal.x );
		Plane plane;
		if(bestCoords.size() > 0)
		{
			
			
			// fit plane to whole points
			linear_least_squares_fitting_3(bestCoords.begin(),bestCoords.end(),plane,CGAL::Dimension_tag<0>());

			//cout << plane <<endl;

			// fit line to triangle vertices
			//linear_least_squares_fitting_3(bestCoords.begin(),bestCoords.end(),line, CGAL::Dimension_tag<0>());
			//cout << line << endl;	
		}  	
		
		//calcula angulo entre os planos	
		vector3f myvec_normal(plane.a(),plane.b(),plane.c());

		float dot = dotProduct(vec_normal,myvec_normal);
		dot = dot / (myvec_normal.length() *  vec_normal.length());
		float angle = acos(dot)*180/3.14159265359;
		//

		// distancia entre os vertices dos planos

		float p0 = -(myvec_normal.z*1.0f  + myvec_normal.y*1.0f + plane.d())/(myvec_normal.x);
		vector3f pl0v0(1,1,p0);
		float p1 = -(myvec_normal.z*1.0f  + myvec_normal.y*-1.0f + plane.d())/(myvec_normal.x);
		vector3f pl0v1(1,1,p1);
		float p2 = -(myvec_normal.z*-1.0f + myvec_normal.y*-1.0f + plane.d())/(myvec_normal.x);
		vector3f pl0v2(1,1,p2);
		float p3 = -(myvec_normal.z*-1.0f + myvec_normal.y*1.0f + plane.d())/(myvec_normal.x);
		vector3f pl0v3(1,1,p3);


		p0 = -(vec_normal.z*1.0f  + vec_normal.y*1.0f + plane_d )/(vec_normal.x);
		vector3f pl1v0(1,1,p0);
		p1 = -(vec_normal.z*1.0f  + vec_normal.y*-1.0f + plane_d)/(vec_normal.x);
		vector3f pl1v1(1,1,p1);
		p2 = -(vec_normal.z*-1.0f + vec_normal.y*-1.0f + plane_d)/(vec_normal.x);
		vector3f pl1v2(1,1,p2);
		p3 = -(vec_normal.z*-1.0f + vec_normal.y*1.0f + plane_d)/(vec_normal.x);
		vector3f pl1v3(1,1,p3);


		float v0_d =  sqrt(pow((pl0v0.x - pl1v0.x),2) + pow((pl0v0.y - pl1v0.y),2) + pow((pl0v0.z - pl1v0.z),2));
		float v1_d =  sqrt(pow((pl0v1.x - pl1v1.x),2) + pow((pl0v1.y - pl1v1.y),2) + pow((pl0v1.z - pl1v1.z),2));
		float v2_d =  sqrt(pow((pl0v2.x - pl1v2.x),2) + pow((pl0v2.y - pl1v2.y),2) + pow((pl0v2.z - pl1v2.z),2));		
		float v3_d =  sqrt(pow((pl0v3.x - pl1v3.x),2) + pow((pl0v3.y - pl1v3.y),2) + pow((pl0v3.z - pl1v3.z),2));		

		v0_d = abs(v0_d);
		v1_d = abs(v1_d);
		v2_d = abs(v2_d);
		v3_d = abs(v3_d);


		float planeDistance = ( pow(v0_d,2) + pow(v1_d,2) + pow(v2_d,2) + pow(v3_d,2) )/ 4;


		printf("%f\n",planeDistance );
		printf("%f\n",angle );

		ofs << planeDistance <<" "<< angle <<" "<< t2-t1 << endl;

		//ofs << vec_normal.z << " "<< vec_normal.y << " " << vec_normal.x << endl;
		//ofs << t2-t1 <<endl <<endl;

		 //  	for(int i = 0; i < PLANES; i++)
		 //  	{
		 //  		printf("%d=>%d,%d\n",i,counts[i][0],counts[i][1]);
		 //  	}
			// printf("%d,%d,%d\n",count2,summ,count3 );

		for (int i=0; i < PP_RAW.resDepth; i++)
		{
			free(simVolume[i]); 
			free(bestMatches[i]);
		}
		free(simVolume);
		free(bestMatches);
		
		free(data4);

		simVolume=NULL;
		bestMatches=NULL;
		data4=NULL;


	}
	ofs.close();
	return 0;		
}
