#include "/ozgan/programs/h/genel.h"

typedef struct ekDATA {
	char kod[10];
	char isim[20];
	char soyisim[20];
	char adres[45];
	char telefon[15];
} DATA;

typedef struct ekIND_DATA {
	char kod[10];
	char isim[20];
} IND_DATA;

TABLO_FORM tablocu[5] = { {Data_Acilis_Bilgisi, YAZI, 4, 20, 10, BEYAZ_ | A_BEYAZ, BEYAZ_ | GRI, Data_Indeks_Bilgisi, SONRA},
						  {Data_Acilis_Bilgisi, YAZI, 4, 20, 20, BEYAZ_ | A_BEYAZ, BEYAZ_ | GRI, Data_Indeks_Bilgisi, SONRA},
						  {Data_Acilis_Bilgisi, YAZI, 4, 20, 20, BEYAZ_ | A_BEYAZ, BEYAZ_ | GRI, Data_Bos_Referans, SONRA},
						  {Data_Acilis_Bilgisi, YAZI, 4, 20, 45, BEYAZ_ | A_BEYAZ, BEYAZ_ | GRI, Data_Bos_Referans, SONRA},
						  {Data_Acilis_Bilgisi, YAZI, 4, 20, 15, BEYAZ_ | A_BEYAZ, BEYAZ_ | GRI, Data_Bos_Referans, SONRA}
						};
IND_DATA *ind;
DATA data;
TABLO_BILGI baslikci = {5, sizeof(DATA), 1, 0};
/*-------------------------------------------------------------------------*/

void yol_yapici(char *yol, char *dosya_adi, char *birlesen)
{
	strcpy(birlesen, yol);

	if (birlesen[strlen(birlesen)-1] != '/') {
		birlesen[strlen(birlesen)] = '/';
	}

	strcat(birlesen, dosya_adi);
}
FILE *ana_dosya_ac(FILE *fp, DOSYA *handle, char *birlesen, char *durumu)
{
	yol_yapici(handle->yol, handle->ana_dosya, birlesen);
	if ((fp = _fsopen(birlesen, durumu, SH_DENYNO)) == NULL) {
		msj_kutu(handle->ana_dosya, HATA_DOSYA_BULUNAMADI, HATA);
		exit(1);
	}
	return fp;
}

FILE *indeks_dosya_ac(FILE *fp, DOSYA *handle, char *birlesen, char *durumu)
{
	yol_yapici(handle->yol, handle->indeks_dosya, birlesen);
	if ((fp = _fsopen(birlesen, durumu, SH_DENYNO)) == NULL) {
		msj_kutu(handle->indeks_dosya, HATA_DOSYA_BULUNAMADI, HATA);
		exit(1);
	}
	return fp;
}

void data_dosyasini_kapa(FILE *fp)
{
	fclose(fp);
}

int indeks_uzunlugu(DOSYA *handle, char *bellek)
{
	int i, indeks_data_uzunlugu = 0, data_yeri = 0;

	for (i = 0; i < handle->baslik.eleman_sayisi; i++) {
		if (handle->tablo[i].referans == Data_Indeks_Bilgisi) {
			if (bellek != NULL)
				memcpy(&bellek[indeks_data_uzunlugu], &handle->ana_detay->data[data_yeri], handle->tablo[i].uzunluk + 1);
			indeks_data_uzunlugu += handle->tablo[i].uzunluk + 1;
		}
		data_yeri += handle->tablo[i].uzunluk + 1;
	}
	return indeks_data_uzunlugu;
}

void baslik_olustur(DOSYA *handle, int durum)
{
	FILE *fp;
	char birlesen[100];

	memset(birlesen, 0, 100);

	switch (durum) {
		case OLUSTUR:
			/******************** ANA BALIK DOLDURULUYOR ********************/
			memset(handle->ana_baslik->dosya_tanimi, 0, 30);
			strcpy(handle->ana_baslik->dosya_tanimi, strupr(handle->ana_dosya));
			strcat(handle->ana_baslik->dosya_tanimi, " DATA DOSYASI");
			handle->ana_baslik->dosya_tanimi[29] = '\x1a';
			handle->ana_baslik->bir_datanin_uzunlugu = handle->baslik.data_uzunlugu;
			handle->ana_baslik->toplam_kayit = 0;
			strcpy(handle->ana_baslik->indeks_dosya_adi, handle->indeks_dosya);
			fp = ana_dosya_ac(fp, handle, birlesen, "wb+");
			fseek(fp, 0, SEEK_SET);
			fwrite(handle->ana_baslik, 1, sizeof(ANA_BASLIK), fp);
			data_dosyasini_kapa(fp);
			/*------------------------ A€IKLAMA SONU ------------------------*/

			/****************** INDEKS BALIK DOLDURULUYOR *******************/
			memset(handle->indeks_baslik->dosya_tanimi, 0, 30);
			strcpy(handle->indeks_baslik->dosya_tanimi, strupr(handle->indeks_dosya));
			strcat(handle->indeks_baslik->dosya_tanimi, " INDEKS DOSYASI");
			handle->indeks_baslik->dosya_tanimi[29] = '\x1a';
			handle->indeks_baslik->indeks_data_uzunlugu = indeks_uzunlugu(handle, NULL);
			handle->indeks_baslik->toplam_kayit = 0;
			handle->indeks_baslik->ilk_kayit = sizeof(INDEKS_BASLIK) 	 +
									   sizeof(INDEKS_DETAY)  - 2 +
									   handle->indeks_baslik->indeks_data_uzunlugu;
			fp = indeks_dosya_ac(fp, handle, birlesen, "wb+");
			fseek(fp, 0, SEEK_SET);
			fwrite(handle->indeks_baslik, 1, sizeof(INDEKS_BASLIK), fp);
			data_dosyasini_kapa(fp);
			/*------------------------ A€IKLAMA SONU ------------------------*/
			break;
		case GUNCELLE:
			/************ ANA DOSYA GUNCELLEN˜YOR ************/
			fp = ana_dosya_ac(fp, handle, birlesen, "rb+");
			fseek(fp, 0, SEEK_SET);
			fwrite(handle->ana_baslik, 1, sizeof(ANA_BASLIK), fp);
			data_dosyasini_kapa(fp);
			/*----------------- A€IKLAMA SONU ---------------*/
			/************* INDEKS DOSYA GUNCELLEN˜YOR *************/
			fp = indeks_dosya_ac(fp, handle, birlesen, "rb+");
			fseek(fp, 0, SEEK_SET);
			fwrite(handle->indeks_baslik, 1, sizeof(INDEKS_BASLIK), fp);
			data_dosyasini_kapa(fp);
			/*------------------- A€IKLAMA SONU -------------------*/
			break;
	}
}

void dosya_yarat(DOSYA *handle)
{
	handle->ana_baslik = (ANA_BASLIK *) malloc(sizeof(ANA_BASLIK));
	handle->indeks_baslik = (INDEKS_BASLIK *) malloc(sizeof(INDEKS_BASLIK));

	if (handle->ana_baslik == NULL || handle->indeks_baslik == NULL) {
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	baslik_olustur(handle, OLUSTUR);
	free(handle->ana_baslik);
	free(handle->indeks_baslik);
}

void baslik_oku(DOSYA *handle)
{
	FILE *fp;
	char birlesen[100];

	/******* ANA DOSYA BASLIK B˜LG˜S˜ OKUNUYOR *******/
	fp = ana_dosya_ac(fp, handle, birlesen, "rb");
	fseek(fp, 0, SEEK_SET);
	fread(handle->ana_baslik, 1, sizeof(ANA_BASLIK), fp);
	data_dosyasini_kapa(fp);
	/*---------------A€IKLAMA SONU-------------------*/
	/******* INDEKS DOSYA BASLIK B˜LG˜S˜ OKUNUYOR *******/
	fp = indeks_dosya_ac(fp, handle, birlesen, "rb");
	fseek(fp, 0, SEEK_SET);
	fread(handle->indeks_baslik, 1, sizeof(INDEKS_BASLIK), fp);
	data_dosyasini_kapa(fp);
	/*-----------------A€IKLAMA SONU--------------------*/
}

void detay_oku(DOSYA *handle, long indeks_detay_adres, int kontrol)
{
	FILE *fp;
	char birlesen[100];

	/******* INDEKS DOSYA DETAY B˜LG˜S˜ OKUNUYOR *******/
	fp = indeks_dosya_ac(fp, handle, birlesen, "rb");
	fseek(fp, indeks_detay_adres, SEEK_SET);
	fread(handle->indeks_detay, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	if (kontrol == DATA_AL)
		fread(handle->indeks_detay->data, 1, handle->indeks_baslik->indeks_data_uzunlugu, fp);
	data_dosyasini_kapa(fp);
	/*----------------A€IKLAMA SONU-------------------*/
	/******* ANA DOSYA DETAY B˜LG˜S˜ OKUNUYOR *******/
	fp = ana_dosya_ac(fp, handle, birlesen, "rb");
	fseek(fp, handle->indeks_detay->ana_dosya_adresi, SEEK_SET);
	fread(handle->ana_detay, 1, sizeof(ANA_DETAY)-sizeof(char *), fp);
	if (kontrol == DATA_AL)
		fread(handle->ana_detay->data, 1, handle->ana_baslik->bir_datanin_uzunlugu, fp);
	data_dosyasini_kapa(fp);
	/*---------------A€IKLAMA SONU------------------*/
}

void dosya_ac(DOSYA *handle)
{
	handle->ana_baslik = (ANA_BASLIK *) malloc(sizeof(ANA_BASLIK));
	if (handle->ana_baslik == 0) {
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	handle->indeks_baslik = (INDEKS_BASLIK *) malloc(sizeof(INDEKS_BASLIK));
	if (handle->indeks_baslik == 0) {
		free(handle->ana_baslik);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	handle->ana_detay = (ANA_DETAY *) malloc(sizeof(ANA_DETAY));
	if (handle->ana_detay == 0) {
		free(handle->ana_baslik);
		free(handle->indeks_baslik);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);

	}
	handle->indeks_detay = (INDEKS_DETAY *) malloc(sizeof(INDEKS_DETAY));
	if (handle->ana_detay == 0) {
		free(handle->ana_baslik);
		free(handle->indeks_baslik);
		free(handle->ana_detay);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	memset(handle->ana_detay, 0, sizeof(ANA_DETAY));
	memset(handle->indeks_detay, 0, sizeof(INDEKS_DETAY));
	memset(handle->ana_baslik, 0, sizeof(ANA_BASLIK));
	memset(handle->indeks_baslik, 0, sizeof(INDEKS_BASLIK));
	baslik_oku(handle);
	handle->indeks_detay->data = (char *) malloc(handle->indeks_baslik->indeks_data_uzunlugu);
	if (handle->ana_detay == 0) {
		free(handle->ana_baslik);
		free(handle->indeks_baslik);
		free(handle->ana_detay);
		free(handle->indeks_detay);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	memset(handle->indeks_detay->data, 0, handle->indeks_baslik->indeks_data_uzunlugu);
	handle->ana_detay->data = (char *) malloc(handle->ana_baslik->bir_datanin_uzunlugu);
	if (handle->ana_detay == 0) {
		free(handle->ana_baslik);
		free(handle->ana_detay->data);
		free(handle->ana_baslik);
		free(handle->indeks_baslik);
		free(handle->ana_detay);
		free(handle->indeks_detay);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	memcpy(handle->ana_detay->data, handle->data, handle->ana_baslik->bir_datanin_uzunlugu);
}

void dosya_kapa(DOSYA *handle)
{
	baslik_olustur(handle, GUNCELLE);
	free(handle->indeks_detay->data);
	free(handle->ana_detay->data);
	free(handle->ana_baslik);
	free(handle->indeks_baslik);
	free(handle->ana_detay);
	free(handle->indeks_detay);
}

const char *kiyas_bilgisi(DOSYA *handle) // indeksi tutulacak olan datanin
{                                         // bir kopyasn oluŸturur.
	int i, data_yeri = 0;

	for (i = 0; i < handle->baslik.data_uzunlugu; i++) {
		if (handle->tablo[i].referans == Data_Indeks_Bilgisi)
			goto CIK;
		data_yeri += handle->tablo[i].uzunluk + 1;
	}
CIK:
	return &handle->ana_detay->data[data_yeri];
}

int kayit_kontrol(DOSYA *handle)
{
	long toplam_kayit;
	toplam_kayit = handle->indeks_baslik->toplam_kayit;

	switch (toplam_kayit) {
		case UYARI_DOSYADA_KAYIT_YOK: return UYARI_DOSYADA_KAYIT_YOK;
		case DOSYADA_BIR_KAYIT_VAR: return DOSYADA_BIR_KAYIT_VAR;
	}
	return DOSYADA_KAYIT_BIRDEN_FAZLA;
}

void eski_kayit(DOSYA *handle, long adres)
{
	FILE *fp;
	char birlesen[100];

	/**************** INDEKS DETAY KAYIT G˜R˜L˜YOR ******************/
	fp = indeks_dosya_ac(fp, handle, birlesen, "rb+");
	fseek(fp, adres, SEEK_SET);
	fwrite(handle->indeks_detay, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	fwrite(handle->indeks_detay->data, 1, handle->indeks_baslik->indeks_data_uzunlugu, fp);
	data_dosyasini_kapa(fp);
	/*----------------------- INDEKS KAYIT SONU ----------------------*/
	/*************** ANA DETAYA KAYIT G˜R˜L˜YOR ******************/
	fp = ana_dosya_ac(fp, handle, birlesen, "rb+");
	fseek(fp, handle->indeks_detay->ana_dosya_adresi, SEEK_SET);
	fwrite(handle->ana_detay, 1, sizeof(ANA_DETAY)-sizeof(char *), fp);
	fwrite(handle->ana_detay->data, 1, handle->ana_baslik->bir_datanin_uzunlugu, fp);
	data_dosyasini_kapa(fp);
	/*-------------------- ANA_DETAY KAYIT SONU -------------------*/
}

long dosya_kayit(DOSYA *handle)
{
	FILE *fp;
	long yeni_indeks_adresi, yeni_ana_adres;
	long sonraki_indeks_adresi;
	char str1[80], str2[80], *data_onceki, birlesen[100];
	INDEKS_DETAY *onceki_indeks;

	dosya_ac(handle);
	/************** D˜Z˜LER TEM˜ZLEN˜YOR **************/
	data_onceki = (char *) malloc(handle->indeks_baslik->indeks_data_uzunlugu);
	if (data_onceki == 0) {
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	onceki_indeks = (INDEKS_DETAY *) malloc(sizeof(INDEKS_DETAY));
	if (onceki_indeks == 0) {
		free(data_onceki);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}

	memset(onceki_indeks, 0, sizeof(INDEKS_DETAY));
	memset(data_onceki, 0, handle->indeks_baslik->indeks_data_uzunlugu);
	memset(str1, 0, 80);
	memset(str2, 0, 80);
	/*-------------- D˜Z˜LER TEM˜ZLEND˜ --------------*/
	if (kayit_kontrol(handle) != UYARI_DOSYADA_KAYIT_YOK) {
		sonraki_indeks_adresi = handle->indeks_baslik->ilk_kayit;
		while (sonraki_indeks_adresi != SON_KAYIT) {
			fp = indeks_dosya_ac(fp, handle, birlesen, "rb");
			fseek(fp, sonraki_indeks_adresi, SEEK_SET);
			fread(onceki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
			fread(data_onceki, 1, handle->indeks_baslik->indeks_data_uzunlugu, fp);
			data_dosyasini_kapa(fp);
			strcpy(str1, kiyas_bilgisi(handle)); // 1. kyaslama bilgisi alnyor.
			strlwrtr(str1); // 1. kyaslama bilgisi trk‡eye g”re k‡ltlyor.
			strcpy(str2, data_onceki); // 2. kyaslama bilgisi alnyor.
			strlwrtr(str2); // 2. kyaslama bilgisi trk‡eye g”re k‡ltlyor.
			if (strcmptr(str1, str2) == 0) {
				free(data_onceki);
				free(onceki_indeks);
				dosya_kapa(handle);
				return 0; // ayn kayttan ikitane olamaz /* !!! HATA !!! */
			}
			else
				sonraki_indeks_adresi = onceki_indeks->sonraki;
		}
	}
	/*************** ANA DETAYA KAYIT G˜R˜L˜YOR ******************/
	fp = ana_dosya_ac(fp, handle, birlesen, "rb+");
	fseek(fp, 0, SEEK_END);
	yeni_ana_adres = ftell(fp);
	fwrite(handle->ana_detay, 1, sizeof(ANA_DETAY)-sizeof(char *), fp);
	fwrite(handle->ana_detay->data, 1, handle->ana_baslik->bir_datanin_uzunlugu, fp);
	data_dosyasini_kapa(fp);
	/*-------------------- ANA_DETAY KAYIT SONU -------------------*/
	/**************** INDEKS DETAY KAYIT G˜R˜L˜YOR ******************/
	fp = indeks_dosya_ac(fp, handle, birlesen, "rb+");
	fseek(fp, 0, SEEK_END);
	yeni_indeks_adresi = ftell(fp);
	handle->indeks_detay->ana_dosya_adresi = yeni_ana_adres;
	if (kayit_kontrol(handle) == UYARI_DOSYADA_KAYIT_YOK) {
		handle->indeks_baslik->ilk_kayit = yeni_indeks_adresi;
		handle->indeks_baslik->son_kayit = yeni_indeks_adresi;
		handle->indeks_detay->onceki = ILK_KAYIT;
		handle->indeks_detay->sonraki = SON_KAYIT;
	}
	indeks_uzunlugu(handle, handle->indeks_detay->data);
	fwrite(handle->indeks_detay, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	fwrite(handle->indeks_detay->data, 1, handle->indeks_baslik->indeks_data_uzunlugu, fp);
	data_dosyasini_kapa(fp);
	/*----------------------- INDEKS KAYIT SONU ----------------------*/
	if (kayit_kontrol(handle) >= 1)
		bul_yerlestir(handle, yeni_indeks_adresi);
	else {
		handle->ana_baslik->toplam_kayit++;
		handle->indeks_baslik->toplam_kayit++;
	}
	free(data_onceki);
	free(onceki_indeks);
	dosya_kapa(handle);
	return yeni_indeks_adresi;
}

int bul_yerlestir(DOSYA *handle, long yeni_indeks_adresi)
{
	FILE *fp;
	char str1[80], str2[80], *data_onceki, birlesen[100];
	INDEKS_DETAY *onceki_indeks;
	INDEKS_DETAY *sonraki_indeks;
	long onceki_indeks_adresi;
	long sonraki_indeks_adresi;

	/************** D˜Z˜LER TEM˜ZLEN˜YOR **************/
	data_onceki = (char *) malloc(handle->indeks_baslik->indeks_data_uzunlugu);
	if (data_onceki == 0) {
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	onceki_indeks = (INDEKS_DETAY *) malloc(sizeof(INDEKS_DETAY));
	if (onceki_indeks == 0) {
		free(data_onceki);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	sonraki_indeks = (INDEKS_DETAY *) malloc(sizeof(INDEKS_DETAY));
	if (sonraki_indeks == 0) {
		free(data_onceki);
		free(onceki_indeks);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	memset(onceki_indeks, 0, sizeof(INDEKS_DETAY));
	memset(sonraki_indeks, 0, sizeof(INDEKS_DETAY));
	memset(str1, 0, 80);
	memset(str2, 0, 80);
	memset(data_onceki, 0, handle->indeks_baslik->indeks_data_uzunlugu);
	/*-------------- D˜Z˜LER TEM˜ZLEND˜ --------------*/

	/*********** KIYASLAMA YAPILIYOR ***********/
	sonraki_indeks_adresi = handle->indeks_baslik->ilk_kayit;

	while (sonraki_indeks_adresi != SON_KAYIT) {
		fp = indeks_dosya_ac(fp, handle, birlesen, "rb");
		fseek(fp, sonraki_indeks_adresi, SEEK_SET);
		onceki_indeks_adresi = ftell(fp);
		fread(onceki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
		fread(data_onceki, 1, handle->indeks_baslik->indeks_data_uzunlugu, fp);
		data_dosyasini_kapa(fp);
		strcpy(str1, kiyas_bilgisi(handle)); // 1. kyaslama bilgisi alnyor.
		strlwrtr(str1); // 1. kyaslama bilgisi trk‡eye g”re k‡ltlyor.
		strcpy(str2, data_onceki); // 2. kyaslama bilgisi alnyor.
		strlwrtr(str2); // 2. kyaslama bilgisi trk‡eye g”re k‡ltlyor.
		if (strcmptr(str1, str2) == 0) {
			free(data_onceki);
			free(onceki_indeks);
			free(sonraki_indeks);
			return 0; // ayn kayttan ikitane olamaz /* !!! HATA !!! */
		}
		else if (strcmptr(str2, str1) < 0)
			sonraki_indeks_adresi = onceki_indeks->sonraki;
		else {
			sonraki_indeks_adresi = onceki_indeks->sonraki;
			// onceki_indeks_adresi girildi.
			goto SON;
		}
	}
	if (sonraki_indeks_adresi == SON_KAYIT) {
		sonraki_indeks->silme_isareti = SILINDI;
		handle->indeks_detay->onceki = handle->indeks_baslik->son_kayit;
		handle->indeks_baslik->son_kayit = yeni_indeks_adresi;
		onceki_indeks->sonraki = yeni_indeks_adresi;
		handle->indeks_detay->sonraki = SON_KAYIT;
		goto ILERLE;
	}
SON:
	if (onceki_indeks->onceki == ILK_KAYIT) {
		onceki_indeks->onceki = yeni_indeks_adresi;
		memcpy(sonraki_indeks, onceki_indeks, sizeof(INDEKS_DETAY)-sizeof(char *));
		onceki_indeks->silme_isareti = SILINDI;
		handle->indeks_detay->sonraki = onceki_indeks_adresi;
		handle->indeks_baslik->ilk_kayit = yeni_indeks_adresi;
		handle->indeks_detay->onceki = ILK_KAYIT;
		sonraki_indeks_adresi =	onceki_indeks_adresi;
		goto ILERLE;
	}
	else {
		memcpy(sonraki_indeks, onceki_indeks, sizeof(INDEKS_DETAY)-sizeof(char *));
		fp = indeks_dosya_ac(fp, handle, birlesen, "rb");
		fseek(fp, sonraki_indeks->onceki, SEEK_SET);
		onceki_indeks_adresi = sonraki_indeks->onceki;
		fread(onceki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
		sonraki_indeks_adresi = onceki_indeks->sonraki;
		data_dosyasini_kapa(fp);

		handle->indeks_detay->onceki = onceki_indeks_adresi;
		handle->indeks_detay->sonraki = sonraki_indeks_adresi;
		onceki_indeks->sonraki = yeni_indeks_adresi;
		sonraki_indeks->onceki = yeni_indeks_adresi;
	}

ILERLE:
	fp = indeks_dosya_ac(fp, handle, birlesen, "rb+");
	if (onceki_indeks->silme_isareti != SILINDI) {
		fseek(fp, onceki_indeks_adresi, SEEK_SET);
		fwrite(onceki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	}
	if (sonraki_indeks->silme_isareti != SILINDI) {
		fseek(fp, sonraki_indeks_adresi, SEEK_SET);
		fwrite(sonraki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	}
	fseek(fp, yeni_indeks_adresi, SEEK_SET);
	fwrite(handle->indeks_detay, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	data_dosyasini_kapa(fp);

	handle->ana_baslik->toplam_kayit++;
	handle->indeks_baslik->toplam_kayit++;

	free(data_onceki);
	free(onceki_indeks);
	free(sonraki_indeks);
	return 1;
}


int data_sil(DOSYA *handle, long silinecek_indeks_adresi)
{
	FILE *fp;
	char birlesen[100];
	INDEKS_DETAY *onceki_indeks;
	INDEKS_DETAY *sonraki_indeks;
	long onceki_indeks_adresi;
	long sonraki_indeks_adresi;

	/************** D˜Z˜LER TEM˜ZLEN˜YOR **************/
	onceki_indeks = (INDEKS_DETAY *) malloc(sizeof(INDEKS_DETAY));
	if (onceki_indeks == 0) {
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	sonraki_indeks = (INDEKS_DETAY *) malloc(sizeof(INDEKS_DETAY));
	if (sonraki_indeks == 0) {
		free(onceki_indeks);
		msj_kutu(NULL, HATA_BELLEK_YETERSIZ, HATA);
		exit(1);
	}
	memset(onceki_indeks, 0, sizeof(INDEKS_DETAY));
	memset(sonraki_indeks, 0, sizeof(INDEKS_DETAY));
	/*-------------- D˜Z˜LER TEM˜ZLEND˜ --------------*/

	/*********** KIYASLAMA YAPILIYOR ***********/
	detay_oku(handle, silinecek_indeks_adresi, DATA_AL);
	sonraki_indeks_adresi = handle->indeks_detay->sonraki;
	onceki_indeks_adresi = handle->indeks_detay->onceki;

	fp = indeks_dosya_ac(fp, handle, birlesen, "rb");
	fseek(fp, sonraki_indeks_adresi, SEEK_SET);
	fread(sonraki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	data_dosyasini_kapa(fp);

	fp = indeks_dosya_ac(fp, handle, birlesen, "rb");
	fseek(fp, onceki_indeks_adresi, SEEK_SET);
	fread(onceki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	data_dosyasini_kapa(fp);

	if (handle->indeks_detay->onceki == ILK_KAYIT) {
		onceki_indeks->silme_isareti = SILINDI;
		sonraki_indeks->onceki = ILK_KAYIT;
		handle->indeks_baslik->ilk_kayit = sonraki_indeks_adresi;
	} else if (handle->indeks_detay->sonraki == SON_KAYIT) {
		sonraki_indeks->silme_isareti = SILINDI;
		onceki_indeks->sonraki = SON_KAYIT;
		handle->indeks_baslik->son_kayit = onceki_indeks_adresi;
	}
	else {
		sonraki_indeks->onceki = onceki_indeks_adresi;
		onceki_indeks->sonraki = sonraki_indeks_adresi;
	}
			/********* DOSYAYA YAZILIYOR ***********/
	fp = indeks_dosya_ac(fp, handle, birlesen, "rb+");

	if (onceki_indeks->silme_isareti != SILINDI) {
		fseek(fp, onceki_indeks_adresi, SEEK_SET);
		fwrite(onceki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	}
	if (sonraki_indeks->silme_isareti != SILINDI) {
		fseek(fp, sonraki_indeks_adresi, SEEK_SET);
		fwrite(sonraki_indeks, 1, sizeof(INDEKS_DETAY)-sizeof(char *), fp);
	}
	data_dosyasini_kapa(fp);

	handle->ana_baslik->toplam_kayit--;
	handle->indeks_baslik->toplam_kayit--;

	free(onceki_indeks);
	free(sonraki_indeks);
	return 1;
}

int konumlan(FILE *fp, long adres) // dosyada istedi§i adrese konumlanr.
{
	fseek(fp, 0, SEEK_SET);
	fseek(fp, adres, SEEK_CUR);
	return ftell(fp);
}

long indeks_gezgini(DOSYA *handle, long ilerlet)
{
	long adres = 0, sayac = 0;

	adres = handle->indeks_baslik->ilk_kayit;
	while (sayac < ilerlet) {
		detay_oku(handle, adres, DATA_AL);
		adres = handle->indeks_detay->sonraki;
		sayac++;
	}
	return adres;
}
#if 0
void main2(void)
{
	DOSYA handle;
	FILE *fp;
	char deneme[100];
	long sayac, i, hata;
	memset(deneme, 0, 100);

	handle.tablo = tablocu;
	handle.baslik = baslikci;
	handle.data = (char *) &data;

	clrscr();
	strcpy(handle.ana_dosya, "fihrist.dat");
	strcpy(handle.indeks_dosya, "fihrist.ind");
	strcpy(handle.yol, "data/");

//	dosya_yarat(&handle);
	printf("     Kod ");
	gets(data.kod);
	printf("    Isim ");
	gets(data.isim);
	printf(" Soyisim ");
	gets(data.soyisim);
	printf("   Adres ");
	gets(data.adres);
	printf(" Telefon ");
	gets(data.telefon);
	hata = dosya_kayit(&handle);//*/
	if (hata == 0) {
		printf("Ayn kayttan var\n");
	}
	dosya_ac(&handle);
//	data_sil(&handle, indeks_gezgini(&handle, 0));
	printf("ilk kayt %ld\n", handle.indeks_baslik->ilk_kayit);
	sayac = handle.indeks_baslik->ilk_kayit;
	i = 0;
	while (sayac != SON_KAYIT) {
		ind = (IND_DATA *)handle.indeks_detay->data;
		detay_oku(&handle, sayac);
//		printf("%s %s %s %s %s\n", data.kod, data.isim, data.soyisim, data.adres, data.telefon);
		sayac = handle.indeks_detay->sonraki;
		printf(" %ld - %s %s\n", i, ind->kod, ind->isim);
		i++;
	}
	dosya_kapa(&handle);
}

void main(void)
{
	int sayac;
	char vericik[100];

	main2();
	exit(2);
	memset(vericik, 0, 100);
//	clrscr();
	for (sayac = 0; sayac < 17; sayac++)
		doldur(4+sayac, 14, ' ', 53, BEYAZ_);
	kalin_cerceve(4, 15, 15, 50, BEYAZ_ | A_BEYAZ);
	doldur(5, 17, ' ', 47, KAHVE_);
	harfler(5, 32, " Ykleme  Program ", KAHVE_ | A_BEYAZ);
	ince_cerceve(6, 17, 7, 46, BEYAZ_ | GRI);
	harfler(7, 32, "!!! ™nemli NOT !!!", BEYAZ_ | SIYAH);
	harfler(8, 18, "    Bu program kullanabilmek i‡in aŸa§da ", BEYAZ_ | GRI);
	harfler(9, 18, " verilen numaralarn do§ru bir Ÿekilde firma-", BEYAZ_ | GRI);
	harfler(10, 18, " mza iletilmesi gerekmektedir. ", BEYAZ_ | GRI);
	harfler(11, 18, "    Yukleme numaralarn +90 533 362 35 65 ", BEYAZ_ | GRI);
	harfler(12, 18, " no'lu telefon numarasndan kullanc kodu-", BEYAZ_ | GRI);
	harfler(13, 18, " nuzla birlikte bildirirek alabilirsiniz.", BEYAZ_ | GRI);
	ince_cerceve(14, 17, 4, 46, BEYAZ_ | GRI);
	harf(14, 17, 'Ã', BEYAZ_ | GRI);
	harf(14, 63, '´', BEYAZ_ | GRI);
	harfler(15, 19, "Ykleme No    3A5C-0156-9874-40F1-9B40-16D0", BEYAZ_ | SIYAH);
	harfler(16, 19, "Kullanc Kodu             34-K.0001-072000", BEYAZ_ | SIYAH);
	harfler(18, 19, "Girilecek No           -    -    -    -    ", BEYAZ_ | A_BEYAZ);
	format_yaz(vericik, "####-####-####-####-####", "0123456789ABCDEFabcdef", 18, 38, 25, BEYAZ_ | A_BEYAZ, BEYAZ_ | A_BEYAZ);
	git_xy(23, 80);
}
/*
int islem(void){
	return getkb();
} */
#endif