#ifndef _DATABASE_H_
#define _DATABASE_H_
/*-------------------------------------------------------------------------
							OzganSYS Yazçlçm ûirketi
							   DATABASE KOMUTLARI
							  Baülçk Dosyasç v.1.0
  -------------------------------------------------------------------------*/
#define UYARI_DOSYADA_KAYIT_YOK		0
#define DOSYADA_BIR_KAYIT_VAR		1
#define DOSYADA_KAYIT_BIRDEN_FAZLA  2

/******** DOSYA òûARETLERò *********/
#define ILK_KAYIT	-10
#define SON_KAYIT	-20
/*---------------------------------*/
#define DATA_AL		0
#define DATA_ALMA	1

#define SILINDI		1 // silme iüareti iáin
/********* BASLIK_OLUSTUR(); FONKSòYONUNUN SABòT TANIMLAMALARI **********/
#define OLUSTUR		0
#define GUNCELLE    1
/*----------------------------------------------------------------------*/

typedef struct ekANA_BASLIK {
	char dosya_tanimi[30]; // Dosya kontrol ve tançm kçsmç
	char indeks_dosya_adi[13]; // òndeks dosyasç adi
	int bir_datanin_uzunlugu; // detaydaki data uzunlugu
	long toplam_kayit; // toplam data adedi
} ANA_BASLIK;

typedef struct ekANA_DETAY {
	char silme_isareti; // kaydçn silinip silinmedißini belirtir.
	char *data; // asçl depolanacak data bilgisi
} ANA_DETAY;

typedef struct ekINDEKS_BASLIK {
	char dosya_tanimi[30]; // Dosya kontrol ve tançm kçsmç
	int indeks_data_uzunlugu; // detaydaki bir adet data uzunlugu
	long ilk_kayit; // sirali ilk detay adresi
	long son_kayit; // sirali son detay adresi
	long toplam_kayit; // toplam data adedi
} INDEKS_BASLIK;

typedef struct ekINDEKS_DETAY {
	long onceki; // kendinden onceki kayit
	long sonraki; //kendinden sonraki kayit
	long ana_dosya_adresi; // ana dosya adresini tutar
	char silme_isareti; // kaydçn silinip silinmedißini belirtir.
	char *data; // asçl depolanacak data bilgisi
} INDEKS_DETAY;

typedef struct ekDOSYA {
	char yol[80];
	char ana_dosya[13];
	char indeks_dosya[13];
	ANA_BASLIK *ana_baslik;
	ANA_DETAY *ana_detay;
	INDEKS_BASLIK *indeks_baslik;
	INDEKS_DETAY *indeks_detay;
	TABLO_BILGI baslik;
	TABLO_FORM *tablo;
	char *data;
} DOSYA;

int bul_yerlestir(DOSYA *handle, long yeni_indeks_adresi);
void yol_yapici(char *yol, char *dosya_adi, char *birlesen);
FILE *ana_dosya_ac(FILE *fp, DOSYA *handle, char *birlesen, char *durumu);
FILE *indeks_dosya_ac(FILE *fp, DOSYA *handle, char *birlesen, char *durumu);
void data_dosyasini_kapa(FILE *fp);
int indeks_uzunlugu(DOSYA *handle, char *bellek);
void baslik_olustur(DOSYA *handle, int durum);
void dosya_yarat(DOSYA *handle);
void baslik_oku(DOSYA *handle);
void detay_oku(DOSYA *handle, long indeks_detay_adres, int kontrol);
void dosya_ac(DOSYA *handle);
void dosya_kapa(DOSYA *handle);
const char *kiyas_bilgisi(DOSYA *handle);
int kayit_kontrol(DOSYA *handle);
void eski_kayit(DOSYA *handle, long adres);
long dosya_kayit(DOSYA *handle);
int bul_yerlestir(DOSYA *handle, long yeni_indeks_adresi);
int data_sil(DOSYA *handle, long silinecek_indeks_adresi);
int konumlan(FILE *fp, long adres);
long indeks_gezgini(DOSYA *handle, long ilerlet);
void kmenu_islem(char *data, TABLO_FORM *tablo, TABLO_BILGI *baslik, int sayac);
#endif



