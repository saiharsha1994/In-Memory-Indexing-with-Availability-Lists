#include<stdio.h>
#include<stdlib.h>
#include <string.h>

typedef struct { 
	int key;
	long off;
} index_S;

typedef struct { 
	int siz;
	long off;
} avail_S;

int count_index = 0, count_aval = 0;

int cmpfunc (const void * a, const void * b) {
		index_S * p = (index_S *)a;
		index_S * q = (index_S *)b;
		return (p->key - q->key);
}

int cmpfuncoff (const void * a, const void * b) {
		avail_S * p = (avail_S *)a;
		avail_S * q = (avail_S *)b;
		return (p->off - q->off);
}

int cmpfuncs (const void * a, const void * b) {
		avail_S * p = (avail_S *)a;
		avail_S * q = (avail_S *)b;
		return (q->siz - p->siz);
}

int binarySearch(index_S index_list[], int check_value){
	int left = 0, right = count_index-1, hit = -1;
	while(left<=right){
			int mid = left+(right-left)/2;
                        if(index_list[mid].key == check_value){
                                hit = mid;
                                break;
                        }
                        else if(index_list[mid].key < check_value){
							hit = -1;
							left = mid + 1;
						}
						else if(index_list[mid].key > check_value){
							hit = -1;
							right = mid - 1;
						}
		}
		return hit;	
}

long checkAvalFirstFit(int size, avail_S avail_list[]){
	if(count_aval == 0)	return -1;
	else{
		int i = 0, count = 0;
		for(i=0; i<count_aval; i++)	if(avail_list[i].siz >= (size+sizeof(int)))	return avail_list[i].off;
		return -1;	
	}
}

void firstFit(char studentFileName[],index_S index_list[], avail_S avail_list[]){
	char *buf;
	long rec_off = 0;
	int rec_siz = 0;
	FILE *fp, *ip, *ap, *pp;
	char functionName[100];
	char studentId[50];
	char studentDetails[1000];
	char * pch;
		fp = fopen( "student.db", "a+b" );
		scanf("%s",functionName);
		while(1>0){
			if(strcmp(functionName, "add") == 0){
				scanf("%s",studentId);
				scanf("%s",studentDetails);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					int count = strlen(studentDetails);
					long insert = checkAvalFirstFit(count, avail_list);
					if(insert == -1){
						fseek(fp, 0L, SEEK_END);
						long offset = ftell(fp);
						index_list[count_index].key = atoi(studentId);
						index_list[count_index].off = (long) offset;
						count_index++;
						qsort(index_list, count_index, sizeof(index_S), cmpfunc);
						fwrite(&count , sizeof(int) , 1 , fp );
						fwrite(studentDetails, sizeof(char) , count , fp );
						scanf("%s",functionName);
					}
					else{
						pp = fopen( "student.db", "r+b" );
						int i = 0, j = 0;
						for(i=0; i<count_aval; i++){
							if(insert == avail_list[i].off){
								int value = avail_list[i].siz - (count + sizeof(int));
								if((count+sizeof(int) == avail_list[i].siz)){
									for(j=i; j<count_aval ;j++ ){
										avail_list[j].siz = avail_list[j+1].siz;
										avail_list[j].off = avail_list[j+1].off;
									}
									count_aval--;
								}
								else{
									for(j=i; j<count_aval ;j++ ){
										avail_list[j].siz = avail_list[j+1].siz;
										avail_list[j].off = avail_list[j+1].off;
									}
									avail_list[count_aval-1].siz = value;
									avail_list[count_aval-1].off = insert + count + sizeof(int);
									int sd = value;
									int ds = insert + count + sizeof(int);
								}
								break;
							}
						}
						fseek(pp, insert, SEEK_SET);
						index_list[count_index].key = atoi(studentId);
						index_list[count_index].off = (long) insert;
						count_index++;
						qsort(index_list, count_index, sizeof(index_S), cmpfunc);
						fwrite(&count , sizeof(int) , 1 , pp );
						fwrite(studentDetails, sizeof(char) , count , pp );
						scanf("%s",functionName);
						fclose(pp);
					}
				}
				else{
					printf("Record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "find") == 0){
				scanf("%s",studentId);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					printf("No record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
				else{
					fseek( fp, index_list[a].off, SEEK_SET );
					fread( &rec_siz, sizeof( int ), 1, fp );
					buf= malloc( rec_siz );
					//printf("rec size %d",rec_siz);
					fread( buf, sizeof(char), rec_siz, fp );
					int i = 0;
					for(i=0; i<rec_siz; i++)	printf("%c",buf[i]);
					printf("\n");
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "del") == 0){
				scanf("%s",studentId);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					printf("No record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
				else{
					int i = 0;
					fseek( fp, index_list[a].off, SEEK_SET );
					fread( &rec_siz, sizeof( int ), 1, fp );
					avail_list[count_aval].siz = rec_siz+sizeof(int);
					avail_list[count_aval].off = index_list[a].off;
					count_aval++;
					for(i=a; i<count_index ;i++ ){
						index_list[i].key = index_list[i+1].key;
						index_list[i].off = index_list[i+1].off;
					}
					count_index--;
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "end") == 0){
				int i=0, j=0;
				printf( "Index:\n");
				for(i=0; i<count_index; i++){
					printf( "key=%d: offset=%ld\n", index_list[i].key, index_list[i].off );
				}
				int sum = 0;
				printf( "Availability:\n");
				for(i=0; i<count_aval; i++){
					printf( "size=%d: offset=%ld\n", avail_list[i].siz, avail_list[i].off );
					sum += avail_list[i].siz;
				}
				printf( "Number of holes: %d\n", count_aval );
				printf( "Hole space: %d\n", sum );
				ip = fopen( "index.bin", "wb" ); 
				fwrite( 	index_list, sizeof( index_S ), count_index, ip );
				ap = fopen( "avaliability.bin", "wb" ); 
				fwrite( avail_list, sizeof( avail_S ), count_aval, ap );
				break; 
			}
		}
}

void bestFit(char studentFileName[],index_S index_list[], avail_S avail_list[]){
	char *buf;
	long rec_off = 0;
	int rec_siz = 0;
	FILE *fp, *ip, *ap, *pp;
	char functionName[100];
	char studentId[50];
	char studentDetails[1000];
	char * pch;
		fp = fopen( "student.db", "a+b" );
		scanf("%s",functionName);
		while(1>0){
			if(strcmp(functionName, "add") == 0){
				scanf("%s",studentId);
				scanf("%s",studentDetails);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					qsort(avail_list, count_aval, sizeof(avail_S), cmpfuncoff);
					qsort(avail_list, count_aval, sizeof(avail_S), cmpfunc);
					int count = strlen(studentDetails);
					long insert = checkAvalFirstFit(count, avail_list);
					if(insert == -1){
						fseek(fp, 0L, SEEK_END);
						long offset = ftell(fp);
						index_list[count_index].key = atoi(studentId);
						index_list[count_index].off = (long) offset;
						count_index++;
						qsort(index_list, count_index, sizeof(index_S), cmpfunc);
						fwrite(&count , sizeof(int) , 1 , fp );
						fwrite(studentDetails, sizeof(char) , count , fp );
						scanf("%s",functionName);
					}
					else{
						pp = fopen( "student.db", "r+b" );
						int i = 0, j = 0;
						for(i=0; i<count_aval; i++){
							if(insert == avail_list[i].off){
								int value = avail_list[i].siz - (count + sizeof(int));
								if((count+sizeof(int) == avail_list[i].siz)){
									for(j=i; j<count_aval ;j++ ){
										avail_list[j].siz = avail_list[j+1].siz;
										avail_list[j].off = avail_list[j+1].off;
									}
									count_aval--;
								}
								else{
									for(j=i; j<count_aval ;j++ ){
										avail_list[j].siz = avail_list[j+1].siz;
										avail_list[j].off = avail_list[j+1].off;
									}
									avail_list[count_aval-1].siz = value;
									avail_list[count_aval-1].off = insert + count + sizeof(int);
									int sd = value;
									int ds = insert + count + sizeof(int);
								}
								break;
							}
						}
						fseek(pp, insert, SEEK_SET);
						index_list[count_index].key = atoi(studentId);
						index_list[count_index].off = (long) insert;
						count_index++;
						qsort(index_list, count_index, sizeof(index_S), cmpfunc);
						fwrite(&count , sizeof(int) , 1 , pp );
						fwrite(studentDetails, sizeof(char) , count , pp );
						scanf("%s",functionName);
						fclose(pp);
					}
				}
				else{
					printf("Record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "find") == 0){
				scanf("%s",studentId);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					printf("No record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
				else{
					fseek( fp, index_list[a].off, SEEK_SET );
					fread( &rec_siz, sizeof( int ), 1, fp );
					buf = malloc( rec_siz ); 
					fread( buf, sizeof(char), rec_siz, fp );
					int  i = 0;
					for(i=0; i<rec_siz; i++)	printf("%c",buf[i]);
					printf("\n");
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "del") == 0){
				scanf("%s",studentId);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					printf("No record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
				else{
					int i = 0;
					fseek( fp, index_list[a].off, SEEK_SET );
					fread( &rec_siz, sizeof( int ), 1, fp );
					avail_list[count_aval].siz = rec_siz+sizeof(int);
					avail_list[count_aval].off = index_list[a].off;
					count_aval++;
					for(i=a; i<count_index ;i++ ){
						index_list[i].key = index_list[i+1].key;
						index_list[i].off = index_list[i+1].off;
					}
					count_index--;
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "end") == 0){
				qsort(avail_list, count_aval, sizeof(avail_S), cmpfuncoff);
				qsort(avail_list, count_aval, sizeof(avail_S), cmpfunc);
				int i=0, j=0;
				printf( "Index:\n");
				for(i=0; i<count_index; i++){
					printf( "key=%d: offset=%ld\n", index_list[i].key, index_list[i].off );
				}
				int sum = 0;
				printf( "Availability:\n");
				for(i=0; i<count_aval; i++){
					printf( "size=%d: offset=%ld\n", avail_list[i].siz, avail_list[i].off );
					sum += avail_list[i].siz;
				}
				printf( "Number of holes: %d\n", count_aval );
				printf( "Hole space: %d\n", sum );
				ip = fopen( "index.bin", "wb" ); 
				fwrite( 	index_list, sizeof( index_S ), count_index, ip );
				ap = fopen( "avaliability.bin", "wb" ); 
				fwrite( avail_list, sizeof( avail_S ), count_aval, ap );
				break; 
			}
		}
}

void worstFit(char studentFileName[],index_S index_list[], avail_S avail_list[]){
	char *buf;
	long rec_off = 0;
	int rec_siz = 0;
	FILE *fp, *ip, *ap, *pp;
	char functionName[100];
	char studentId[50];
	char studentDetails[1000];
	char * pch;
		fp = fopen( "student.db", "a+b" );
		scanf("%s",functionName);
		while(1>0){
			if(strcmp(functionName, "add") == 0){
				scanf("%s",studentId);
				scanf("%s",studentDetails);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					qsort(avail_list, count_aval, sizeof(avail_S), cmpfuncoff);
					qsort(avail_list, count_aval, sizeof(avail_S), cmpfuncs);
					int count = strlen(studentDetails);
					long insert = checkAvalFirstFit(count, avail_list);
					if(insert == -1){
						fseek(fp, 0L, SEEK_END);
						long offset = ftell(fp);
						index_list[count_index].key = atoi(studentId);
						index_list[count_index].off = (long) offset;
						count_index++;
						qsort(index_list, count_index, sizeof(index_S), cmpfunc);
						fwrite(&count , sizeof(int) , 1 , fp );
						fwrite(studentDetails, sizeof(char) , count , fp );
						scanf("%s",functionName);
					}
					else{
						pp = fopen( "student.db", "r+b" );
						int i = 0, j = 0;
						for(i=0; i<count_aval; i++){
							if(insert == avail_list[i].off){
								int value = avail_list[i].siz - (count + sizeof(int));
								if((count+sizeof(int) == avail_list[i].siz)){
									for(j=i; j<count_aval ;j++ ){
										avail_list[j].siz = avail_list[j+1].siz;
										avail_list[j].off = avail_list[j+1].off;
									}
									count_aval--;
								}
								else{
									for(j=i; j<count_aval ;j++ ){
										avail_list[j].siz = avail_list[j+1].siz;
										avail_list[j].off = avail_list[j+1].off;
									}
									avail_list[count_aval-1].siz = value;
									avail_list[count_aval-1].off = insert + count + sizeof(int);
									int sd = value;
									int ds = insert + count + sizeof(int);
								}
								break;
							}
						}
						fseek(pp, insert, SEEK_SET);
						index_list[count_index].key = atoi(studentId);
						index_list[count_index].off = (long) insert;
						count_index++;
						qsort(index_list, count_index, sizeof(index_S), cmpfunc);
						fwrite(&count , sizeof(int) , 1 , pp );
						fwrite(studentDetails, sizeof(char) , count , pp );
						scanf("%s",functionName);
						fclose(pp);
					}
				}
				else{
					printf("Record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "find") == 0){
				scanf("%s",studentId);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					printf("No record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
				else{
					fseek( fp, index_list[a].off, SEEK_SET );
					fread( &rec_siz, sizeof( int ), 1, fp );
					buf = malloc( rec_siz ); 
					fread( buf, sizeof(char), rec_siz, fp );
					int i = 0;
					for(i=0; i<rec_siz; i++)	printf("%c",buf[i]);
					printf("\n");
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "del") == 0){
				scanf("%s",studentId);
				int a = binarySearch(index_list,atoi(studentId));
				if(a == -1){
					printf("No record with SID=%d exists\n",atoi(studentId) );
					scanf("%s",functionName);
				}
				else{
					int i = 0;
					fseek( fp, index_list[a].off, SEEK_SET );
					fread( &rec_siz, sizeof( int ), 1, fp );
					avail_list[count_aval].siz = rec_siz+sizeof(int);
					avail_list[count_aval].off = index_list[a].off;
					count_aval++;
					for(i=a; i<count_index ;i++ ){
						index_list[i].key = index_list[i+1].key;
						index_list[i].off = index_list[i+1].off;
					}
					count_index--;
					scanf("%s",functionName);
				}
			}
			else if(strcmp(functionName, "end") == 0){
				int i=0, j=0;
				qsort(avail_list, count_aval, sizeof(avail_S), cmpfuncoff);
				qsort(avail_list, count_aval, sizeof(avail_S), cmpfuncs);
				printf( "Index:\n");
				for(i=0; i<count_index; i++){
					printf( "key=%d: offset=%ld\n", index_list[i].key, index_list[i].off );
				}
				int sum = 0;
				printf( "Availability:\n");
				for(i=0; i<count_aval; i++){
					printf( "size=%d: offset=%ld\n", avail_list[i].siz, avail_list[i].off );
					sum += avail_list[i].siz;
				}
				printf( "Number of holes: %d\n", count_aval );
				printf( "Hole space: %d\n", sum );
				ip = fopen( "index.bin", "wb" ); 
				fwrite( 	index_list, sizeof( index_S ), count_index, ip );
				ap = fopen( "avaliability.bin", "wb" ); 
				fwrite( avail_list, sizeof( avail_S ), count_aval, ap );
				break; 
			}
		}
}


int main(int argc, char *argv[]){
	FILE *ip, *ap;
	index_S *index_list = (index_S *) malloc(10000 * sizeof(index_S));
	avail_S *avail_list = (avail_S *) malloc(10000 * sizeof(avail_S));
	index_S buff1;
	avail_S	buff2;
	if((ip = fopen( "index.bin", "rb" )) == NULL){}	
	else{	
		while(fread(&buff1, sizeof( index_S ), 1, ip)){
			index_list[count_index].key = buff1.key;
			index_list[count_index].off = buff1.off;
			count_index++;
		}
	}
	if((ap = fopen( "avaliability.bin", "rb")) == NULL){}
	else{	
		while(fread(&buff2, sizeof( avail_S ), 1, ap)){
			avail_list[count_aval].siz = buff2.siz;
			avail_list[count_aval].off = buff2.off;
			count_aval++;
		}
	}
	int i=0;
	if ( ( ap = fopen( "available.bin", "r+b" ) ) == NULL )
			ap = fopen( "available.bin", "w+b" );
	if(strcmp( argv[1], "--first-fit") == 0 )
		firstFit(argv[2],index_list, avail_list);
	else if(strcmp( argv[1], "--best-fit") == 0 )
		bestFit(argv[2],index_list, avail_list);
	else if(strcmp( argv[1], "--worst-fit") == 0 )
		worstFit(argv[2],index_list, avail_list);
	else
		printf("Enter Valid Input");
	return 0;
}
