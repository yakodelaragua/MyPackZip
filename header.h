#define TAM_S_HEADER    512
#define MAX_FILE_NAME   256

struct s_info
{
    char Tipo;
    char Compri;

    unsigned long TamOri;
    unsigned long TamComp;

    char FileName[MAX_FILE_NAME];

};

#define TAM_S_INFO sizeof(struct s_info)

#define RESTO_HEADER (TAM_S_HEADER - TAM_S_INFO)

struct s_header
{
    //anadido despues
    long posicion;
    struct s_info InfoF;
    char RestoHeader[RESTO_HEADER];

};

