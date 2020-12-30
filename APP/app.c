#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_menu()
{
	puts("1: Pročitaj trenutno stanje stringa");
	puts("2: Upiši novi string");
	puts("3: Konkataniraj string na trenutni");
	puts("4: Izbriši čitav string");
	puts("5: Izbriši vodeće i prateće space karaktere");
	puts("6: Izbriši izraz iz stringa");
	puts("7: Izbriši poslednjih n karaktera iz stringa");
}


int main(void)
{
	char input = '0';
	char *file_data;
	char *input_data;
	char *shrink = "shrink\n";
	char *clear = "clear\n";
	size_t num_of_bytes = 100;
	FILE* fp;

	while(1)
	{
		print_menu();
		input_data = (char *)malloc(num_of_bytes+1); 
		getline(&input_data, &num_of_bytes, stdin); 
		sscanf(input_data, "%c", &input);
		switch (input)
		{
		
			case '1':
				fp = fopen("/dev/stred", "r");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred");
					return -1;
				}
				file_data = (char *)malloc(num_of_bytes+1); 
				getline(&file_data, &num_of_bytes, fp); 
				puts("Trenutno stanje stringa je:");
				if( *file_data == '\0' )
					puts("** PRAZAN STRING! **");
				else
					puts(file_data);
				free(file_data);
				break;
				
								
			case '2':
				fp = fopen("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred");
					return -1;
				}
				puts("Unesite string za upis");
				file_data = (char *)malloc(num_of_bytes+1); 
				getline(&file_data, &num_of_bytes, stdin); 
				fputs("string=",fp);
				fputs(file_data, fp);
				free(file_data);
				break;
				
			case '3':
				fp = fopen("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred");
					return -1;
				}
				puts("Unesite string za konkatenaciju");
				file_data = (char *)malloc(num_of_bytes+1); 
				getline(&file_data, &num_of_bytes, stdin); 
				fputs("append=",fp);
				fputs(file_data, fp);
				free(file_data);
				break;
				
			case '4':
				fp = fopen("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred");
					return -1;
				}
				fputs(clear,fp);
				puts("String obrisan");
				break;					
				
			case '5':
				fp = fopen("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred");
					return -1;
				}
				fputs(shrink,fp);
				puts("Obrisani vodeci i prateci prazni karakteri");
				break;		
				
			case '6':
				fp = fopen("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred");
					return -1;
				}
				puts("Unesite rec za brisanje iz stringa");
				file_data = (char *)malloc(num_of_bytes+1); 
				getline(&file_data, &num_of_bytes, stdin); 
				fputs("remove=",fp);
				fputs(file_data, fp);
				free(file_data);
				break;
				
			case '7':
				fp = fopen("/dev/stred", "w");
				if(fp==NULL)
				{
					puts("Problem pri otvaranju /dev/stred");
					return -1;
				}
				puts("Unesite broj karaktera koji zelite da bude obrisan s kraja");
				file_data = (char *)malloc(num_of_bytes+1); 
				getline(&file_data, &num_of_bytes, stdin); 
				fputs("truncate=",fp);
				fputs(file_data, fp);
				free(file_data);
				break;
				
			case 'Q':
				puts("Kraj");
				return 0;
				break;
				
			default:
				puts("Nesto je poslo po zlu");
				return 0;
				break;
		}
		
		if(fclose(fp))
		{
			puts("Problem pri zatvaranju /dev/stred");
			return -1;
		}
		free(input_data);
	}			
	return 0;
}

