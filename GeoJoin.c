#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define SIZELINE 8192
#define COL 3
#define ROW 65536
#define MAXSIZEID 48
#define PI 3.14159265358979323846
#define RADIO_TIERRA 6371


char* line_1[SIZELINE];

typedef struct TipoGeoRef {
char ID[MAXSIZEID];
double lat;
double lon;
} TipoGeoRef;


TipoGeoRef *DATA [ROW];
long nro_de_ref=0;

double gradosARadianes(double grados)
{
    return grados * PI / 180;
}


//Recibe dos pares de coordenadas en grados (así como las da Google maps)
//Regresa la distancia que hay entre esos dos puntos en Kilómetros

double calcularDistancia(double lat1, double lon1, double lat2, double lon2)
{
    // Convertir todas las coordenadas a radianes
    lat1 = gradosARadianes(lat1);
    lon1 = gradosARadianes(lon1);
    lat2 = gradosARadianes(lat2);
    lon2 = gradosARadianes(lon2);
    // Aplicar fórmula
    double diferenciaEntreLongitudes = (lon2 - lon1);
    double diferenciaEntreLatitudes = (lat2 - lat1);
    double a = pow(sin(diferenciaEntreLatitudes / 2.0), 2) + cos(lat1) * cos(lat2) * pow(sin(diferenciaEntreLongitudes / 2.0), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = RADIO_TIERRA * c;

    return d;
}



long buscar_referencia_mas_cercana (double lat1, double lon1)
{
long mas_cercana=0;
double menor_distancia=RADIO_TIERRA*2;

double aux;


for (int i=0;i<nro_de_ref;i++)
        {   aux=calcularDistancia(lat1,lon1,DATA[i]->lat,DATA[i]->lon);
            if (aux<menor_distancia) { menor_distancia=aux;
                                        mas_cercana=i;

                                      }
        }

return mas_cercana;

}



long read_csv_ref( char *filename){

    char line[SIZELINE];
    FILE *file;
    long i=0;

	file = fopen(filename, "r");

    if (file!=NULL)
        {
            printf("Loading %s \n",filename);

            fgets(line, SIZELINE, file); //HEADER
            strncpy(line_1,line,MAXSIZEID);

            while (fgets(line, SIZELINE, file) && (i < ROW))
            {
                // double row[ssParams->nreal + 1];
                DATA[i] = (TipoGeoRef*)malloc(sizeof(TipoGeoRef));

                int j = 0;
                const char* tok;
                for (tok = strtok(line, ";"); tok && *tok; j++, tok = strtok(NULL, ";\n"))
                {
                if(j==0) strncpy(DATA[i]->ID,tok,MAXSIZEID);
                else if (j==1)  sscanf(tok, "%lf", &DATA[i]->lat );
                else if (j==2) sscanf(tok, "%lf", &DATA[i]->lon );
                }
              //  printf("ID: %s ; LAT: %lf ; LONG: %lf   \n  ",DATA[i]->ID,DATA[i]->lat,DATA[i]->lon);

                i++;
            }

        }

    else
    {
        printf("Error Loading %s \n",filename);

    }

return i;
}



long read_csv_sities( char *filename){

    char line[SIZELINE];
    FILE *file;
    long i=0;
    long mas_cercana;
    double distancia;

    TipoGeoRef Reg_actual;

	file = fopen(filename, "r");


    if (file!=NULL)
        {
            printf("Loading %s \n",filename);

            fgets(line, SIZELINE, file); //HEADER

            while (fgets(line, SIZELINE, file) && (i < ROW))
            {
                // double row[ssParams->nreal + 1];
                int j = 0;
                const char* tok;
                for (tok = strtok(line, ";"); tok && *tok; j++, tok = strtok(NULL, ";\n"))
                {
                if(j==0) strncpy(Reg_actual.ID,tok,MAXSIZEID);
                else if (j==1)  sscanf(tok, "%lf", &Reg_actual.lat );
                else if (j==2) sscanf(tok, "%lf", &Reg_actual.lon );
                }
               printf("%s;%lf;%lf",Reg_actual.ID,Reg_actual.lat,Reg_actual.lon);

               mas_cercana = buscar_referencia_mas_cercana (Reg_actual.lat,Reg_actual.lon);

               distancia = calcularDistancia (Reg_actual.lat,Reg_actual.lon,DATA[mas_cercana]->lat,DATA[mas_cercana]->lon);
               printf(";%s;%lf;%lf;%lf\n",DATA[mas_cercana]->ID, DATA[mas_cercana]->lat, DATA[mas_cercana]->lon, distancia  );


               i++;
            }

        }

    else
    {
        printf("Error Loading %s \n",filename);

    }

return i;
}


void read_csv(int row, int col, char *filename, double **data){
	FILE *file;
	file = fopen(filename, "r");

	int i = 0;
    char line[SIZELINE];
	while (fgets(line, SIZELINE, file) && (i < row))
    {

	    int j = 0;
	    const char* tok;
	    for (tok = strtok(line, "\t"); tok && *tok; j++, tok = strtok(NULL, "\t\n"))
	    {
	        data[i][j] = atof(tok);
	        printf("%f\t", data[i][j]);
	    }
	    printf("\n");

        i++;
    }
}

int main(int argc, char const *argv[])
{
    printf("GeoJoin 1.0\n");

	/* code */
	if (argc < 1){
		fprintf( stderr,"Usage: GeoJoin <sities_file.csv> <references_file.csv> as an input.\n");
		exit(0);
	}

	int row     = ROW; //atoi(argv[1]);
	int col     = COL; //atoi(argv[2]);

	char fnameSit[256];	strcpy(fnameSit, argv[1]);
	char fnameRef[256];	strcpy(fnameRef, argv[2]);

    nro_de_ref = read_csv_ref (fnameRef);
    fprintf( stderr,"%ld references readed from %s\n",nro_de_ref,fnameRef);


    long sit = read_csv_sities (fnameSit);
    fprintf( stderr,"%ld references readed from %s\n",sit,fnameSit);

    getchar();



	return 0;
}
