#include<stdio.h>
#include<time.h>
#include<string.h>
#include<windows.h>
#include<conio.h>

struct user{
	char role[10], name[100], pass[100];
} userlist[100];

struct book{
	char name[100], auth[50], genre[30], pub[50];
	int year, stock;
	float price;
} booklist[1000];

struct filter{
	int active[7];
	char name[100], auth[50], genre[30], pub[50];
	int yeartop, yearbot, stocktop, stockbot;
	float pricetop, pricebot;
} filterlist;

struct cartitem{
	int index, amount;
} cart[100];

int bookamount=0, useramount=0, showbook[1000]={0}, showbookamount=0, page=0, maxpage=0, cartamount=0, sortby=80, loggeduser=-1;
char color[10], pp[50]="mode con: cols=130 lines=00";
static int maxitemperpage=25, lowstocklimit=5;

void filterbooks();
void changewindowsize();

int loadusers(){
	FILE *pfile=fopen("user.pwd", "r");
	if(pfile==NULL)return -1;
	int ctr;
	for(ctr=0; 1; ctr++){
		if(fscanf(pfile, "%[^■]■%[^■]■%[^\n]\n", &userlist[ctr].role, &userlist[ctr].name, &userlist[ctr].pass)==EOF)break;
		useramount++;
	}
	fclose(pfile);
	return 0;
}

int loaddata(){
	FILE *pfile=fopen("booklist.dat", "r");
	if(pfile==NULL)return -1;
	int ctr;
	for(ctr=0; 1; ctr++){
		if(fscanf(pfile, "%[^■]■%[^■]■%[^■]■%[^■]■%d■%d■%f\n", &booklist[ctr].name, &booklist[ctr].auth, &booklist[ctr].genre, &booklist[ctr].pub, &booklist[ctr].year, &booklist[ctr].stock, &booklist[ctr].price)==EOF)break;
		bookamount++;
	}
	fclose(pfile);
	maxpage=(bookamount-1)/maxitemperpage;
	filterbooks();
	return 0;
}

int loadoption(){
	FILE *pfile=fopen("option.cfg", "r");
	if(pfile==NULL)return -1;
	int ctr;
	fscanf(pfile, "%[^\n]\n", &color);
	fscanf(pfile, "maxitemperpage %d\n", &maxitemperpage);
	fscanf(pfile, "lowstocklimit %d\n", &lowstocklimit);
	changewindowsize();
	fclose(pfile);
	return 0;
}

void saveuser(){
	FILE *pfile;
	int ctr;
	pfile=fopen("user.pwd", "w");
	for(ctr=0; ctr<useramount; ctr++)fprintf(pfile, "%s■%s■%s\n", userlist[ctr].role, userlist[ctr].name, userlist[ctr].pass);
	fclose(pfile);
}

void savedata(){
	FILE *pfile;
	int ctr;
	pfile=fopen("booklist.dat", "w");
	for(ctr=0; ctr<bookamount; ctr++)fprintf(pfile, "%s■%s■%s■%s■%d■%d■%.2f\n", booklist[ctr].name, booklist[ctr].auth, booklist[ctr].genre, booklist[ctr].pub, booklist[ctr].year, booklist[ctr].stock, booklist[ctr].price);
	fclose(pfile);
}

void saveoption(){
	FILE *pfile;
	int ctr;
	pfile=fopen("option.cfg", "w");
	fprintf(pfile, "%s\n", color);
	fprintf(pfile, "maxitemperpage %d\n", maxitemperpage);
	fprintf(pfile, "lowstocklimit %d\n", lowstocklimit);
	fclose(pfile);
}

void drawheader(){
	printf("\n");
	printf("\t\t\t\t  ░░░ ░ ░ ░░░\n");
	printf("\t\t\t\t   ░  ░░░ ░░ \n");
	printf("\t\t\t\t   ░  ░ ░ ░░░\n");
	printf("\t\t\t\t▓▓    ▓▓▓▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓▓▓  ▓▓▓▓▓ ▒▒  ▒▒▒ ▒▒▒ ▒ ▒ ▒▒▒ ▒▒▒ ▒▒▒ ▒▒  ▒▒▒\n");
	printf("\t\t\t\t▓▓    ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓  ▓ ▓▓    ▒ ▒ ▒ ▒ ▒ ▒ ▒ ▒ ▒    ▒  ▒ ▒ ▒ ▒ ▒  \n");
	printf("\t\t\t\t▓▓    ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓▓▓  ▓▓▓▓▓ ▒▒  ▒ ▒ ▒ ▒ ▒▒  ▒▒▒  ▒  ▒ ▒ ▒▒  ▒▒▒\n");
	printf("\t\t\t\t▓▓    ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓ ▓▓    ▒ ▒ ▒ ▒ ▒ ▒ ▒ ▒   ▒  ▒  ▒ ▒ ▒ ▒ ▒  \n");
	printf("\t\t\t\t▓▓▓▓▓ ▓▓▓▓▓ ▓▓▓▓▓  ▓▓▓  ▓▓  ▓ ▓▓▓▓▓ ▒▒  ▒▒▒ ▒▒▒ ▒ ▒ ▒▒▒  ▒  ▒▒▒ ▒ ▒ ▒▒▒\n");
	printf("\t\t\t\t\t\tVersion 0.11.7 Beta (Last updated 19 May 2016)\n");
}

void drawheaders(){
	printf("\t\t  ┌────────────────────────────┐\n");
	printf("\t\t  │   The                      │\n");
	printf("\t\t  │ LOUVRE Bookstore v 0.11.7b │\n");
	printf("\t\t  └────────────────────────────┘\n");
}

char lower(char x){return (x>='A'&&x<='Z')?x+32:x;}
void swap(int *i, int *j){int k=*j;*j=*i;*i=k;}

void gotoxy(short x, short y){
	COORD coord={x,y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int parsestr(char str[], char mask[], int lim){
	int ctr, rep=(strlen(mask)==0?0:1);
	char in;
	strcpy(str, "");
	for(ctr=0; 1;){
		in=getch();
		if(in==8){if(ctr>0){str[--ctr]=0;printf("\b \b");}}
		else if(in==13&&strlen(str)!=0)break;
		else if(in==32&&strlen(str)==0)continue;
		else if(in==27)return -1;
		else if(in>=32&&ctr<lim){
			str[ctr++]=in;
			if(rep)printf("%s", mask);
			else printf("%c", in);
		}
	}
	str[ctr]=0;
	return 0;
}

int parselocstr(char str[], short x, short y, char mask[], int lim){
	gotoxy(x, y);
	return parsestr(str, mask, lim);
}

int parsebet(char *chr, char dom[]){
	int ctr;
	char in;
	*chr=0;
	for(ctr=0; 1;){
		in=getch();
		if(in==8){if(ctr>0){*chr=0;printf("\b \b");ctr--;}}
		else if(in==13&&ctr!=0)break;
		else if(in==32)continue;
		else if(in==27)return -1;
		else if(ctr<1){
			int lgt=0, i;
			for(i=0; i<strlen(dom); i++)if(lower(dom[i])==lower(in)){lgt++;break;}
			if(lgt){*chr=in;printf("%c", in);ctr++;}
		}
	}
	return 0;
}

int parselocbet(char *chr, short x, short y, char dom[]){
	gotoxy(x, y);
	return parsebet(chr, dom);
}

int parseint(int *num, int lim, int allowneg){
	int ctr, neg=0;
	char in, str[100]={0};
	*num=0;
	for(ctr=0; 1;){
		in=getch();
		if(in==8){if(ctr>0){str[--ctr]=0;printf("\b \b");}}
		else if(in==13&&strlen(str)!=0)break;
		else if(in==32)continue;
		else if(in==27)return -1;
		else if(in=='-'&&allowneg&&ctr==0||in>='0'&&in<='9'&&ctr<lim){
			str[ctr++]=in;
			printf("%c", in);
		}
	}
	str[ctr]=0;
	if(str[0]=='-')neg=1;
	for(ctr=neg; str[ctr]; ctr++)*num=*num*10+str[ctr]-'0';
	if(neg)*num*=-1;
	return 0;
}

int parselocint(int *num, short x, short y, int lim, int allowneg){
	gotoxy(x, y);
	return parseint(num, lim, allowneg);
}

int parseflt(float *num, int lim, int dec){
	int ctr, prd=100;
	char in, str[100]={0};
	float multi=1.0, val;
	*num=0.0;
	for(ctr=0; 1;){
		in=getch();
		if(in==8){if(ctr>0){if(str[ctr-1]=='.')prd=100;str[--ctr]=0;printf("\b \b");}}
		else if(in==13&&strlen(str)!=0)break;
		else if(in==32)continue;
		else if(in==27)return -1;
		else if(in=='.'){
			if(prd<lim)continue;
			else{prd=ctr;str[ctr++]=in;printf("%c", in);}
		}
		else if(in>='0'&&in<='9'&&(prd>lim&&ctr<lim-dec-1||prd<lim&&ctr<prd+dec+1)){
			str[ctr++]=in;
			printf("%c", in);
		}
	}
	str[ctr]=0;
	if(prd>lim)prd=strlen(str);
	for(multi=1.0, ctr=prd-1; ctr>=0; multi*=10.0, ctr--){val=str[ctr]-'0';*num+=multi*val;}
	for(multi=0.1, ctr=prd+1; str[ctr]; multi/=10.0, ctr++){val=str[ctr]-'0';*num+=multi*val;}
	return 0;
}

int parselocflt(float *num, short x, short y, int lim, int dec){
	gotoxy(x, y);
	return parseflt(num, lim, dec);
}

int issubstring(char main[], char sub[]){
	int mainlen=strlen(main), sublen=strlen(sub), ctr1, ctr2;
	for(ctr1=0; ctr1<=mainlen-sublen; ctr1++){
		for(ctr2=0; ctr2<sublen; ctr2++){
			if(lower(main[ctr1+ctr2])!=lower(sub[ctr2]))break;
		}
		if(ctr2==sublen)return 1;
	}
	return 0;
}

int checkqualify(struct book test){
	int ctr1, ctr2;
	if(filterlist.active[0]&&!issubstring(test.name, filterlist.name))return 0;
	if(filterlist.active[1]&&!issubstring(test.auth, filterlist.auth))return 0;
	if(filterlist.active[2]&&!issubstring(test.genre, filterlist.genre))return 0;
	if(filterlist.active[3]&&!issubstring(test.pub, filterlist.pub))return 0;
	if(filterlist.active[4]&&(test.year<filterlist.yearbot||test.year>filterlist.yeartop))return 0;
	if(filterlist.active[5]&&(test.stock<filterlist.stockbot||test.stock>filterlist.stocktop))return 0;
	if(filterlist.active[6]&&(test.price<filterlist.pricebot||test.price>filterlist.pricetop))return 0;
	return 1;
}

void filterbooks(){
	int ctr;
	showbookamount=0;
	for(ctr=0; ctr<bookamount; ctr++){
		if(checkqualify(booklist[ctr]))showbook[showbookamount++]=ctr;
	}
	maxpage=(showbookamount-1)/maxitemperpage;
}

int sortingseq(int i, int j){
	if(sortby==10)return strcmp(booklist[showbook[i]].name, booklist[showbook[j]].name)>0;
	else if(sortby==11)return strcmp(booklist[showbook[i]].name, booklist[showbook[j]].name)<0;
	else if(sortby==20)return strcmp(booklist[showbook[i]].auth, booklist[showbook[j]].auth)>0;
	else if(sortby==21)return strcmp(booklist[showbook[i]].auth, booklist[showbook[j]].auth)<0;
	else if(sortby==30)return strcmp(booklist[showbook[i]].genre, booklist[showbook[j]].genre)>0;
	else if(sortby==31)return strcmp(booklist[showbook[i]].genre, booklist[showbook[j]].genre)<0;
	else if(sortby==40)return strcmp(booklist[showbook[i]].pub, booklist[showbook[j]].pub)>0;
	else if(sortby==41)return strcmp(booklist[showbook[i]].pub, booklist[showbook[j]].pub)<0;
	else if(sortby==50)return booklist[showbook[i]].year>booklist[showbook[j]].year;
	else if(sortby==51)return booklist[showbook[i]].year<booklist[showbook[j]].year;
	else if(sortby==60)return booklist[showbook[i]].stock>booklist[showbook[j]].stock;
	else if(sortby==61)return booklist[showbook[i]].stock<booklist[showbook[j]].stock;
	else if(sortby==70)return booklist[showbook[i]].price>booklist[showbook[j]].price;
	else if(sortby==71)return booklist[showbook[i]].price<booklist[showbook[j]].price;
	else if(sortby==80)return showbook[i]>showbook[j];
	else if(sortby==81)return showbook[i]<showbook[j];
}

void sortbooks(){
	int ctr1, ctr2;
	for(ctr1=0; ctr1<showbookamount-1; ctr1++){
		for(ctr2=ctr1+1; ctr2<showbookamount; ctr2++){
			if(sortingseq(ctr1, ctr2))swap(&showbook[ctr1], &showbook[ctr2]);
		}
	}
}

void errorscreen(int cause){
	system("mode con: cols=65 lines=17");
	system("cls"); drawheaders();
	printf("\n\t\t   An unexpected error occurred.\n\n");
	if(cause==-1){printf("\tBook database is not found in executable directory.\n");}
	else if(cause==-2){printf("\tOptions data  is not found in executable directory.\n");}
	else if(cause==-3){printf("\tUser database is not found in executable directory.\n");}
	printf("\n\t\t    The program will now exit.\n"); getch();
}

void changewindowsize(){
	int lines=maxitemperpage+25;
	pp[25]=pp[26]='0';
	pp[25]+=lines/10;
	pp[26]+=lines%10;
}

void printinvoice(){
	FILE *pfile;
	int ctr;
	float total=0;
	char invoicename[50], invoicetime[50];
	time_t now=time(0);
	struct tm timeinfo=*localtime(&now);
	strftime(invoicename, sizeof(invoicename), "invoice_%Y%m%d_%H%M%S.txt", &timeinfo);
	strftime(invoicetime, sizeof(invoicename), "%H:%M:%S  %d-%m-%Y", &timeinfo);
	pfile=fopen(invoicename, "w");
	fprintf(pfile, "╔══════════════════════════════════════════╗\n");
	fprintf(pfile, "║                                          ║\n");
	fprintf(pfile, "║           THE LOUVRE BOOKSTORE           ║\n");
	fprintf(pfile, "║                                          ║\n");
	fprintf(pfile, "║           Book Puchase Invoice           ║\n");
	fprintf(pfile, "║           %20s           ║\n", invoicetime);
	fprintf(pfile, "║                                          ║\n");
	fprintf(pfile, "╠══════════════════════════════════════════╣\n");
	fprintf(pfile, "║                                          ║\n");
	for(ctr=0; ctr<cartamount; ctr++){
		fprintf(pfile, "║ %-40s ║\n", booklist[cart[ctr].index].name);
		fprintf(pfile, "║   %7d x $%8.2f      = $%10.2f ║\n", cart[ctr].amount, booklist[cart[ctr].index].price, (float)cart[ctr].amount*booklist[cart[ctr].index].price);
		total+=(float)cart[ctr].amount*booklist[cart[ctr].index].price;
	}
	fprintf(pfile, "╟──────────────────────────────────────────╢\n");
	fprintf(pfile, "║  Total                       $%10.2f ║\n", total);
	fprintf(pfile, "║  Tax                         $%10.2f ║\n", total*.1);
	fprintf(pfile, "╟──────────────────────────────────────────╢\n");
	fprintf(pfile, "║  GRAND TOTAL                 $%10.2f ║\n", total*1.1);
	fprintf(pfile, "║                                          ║\n");
	fprintf(pfile, "╠══════════════════════════════════════════╣\n");
	fprintf(pfile, "║                                          ║\n");
	fprintf(pfile, "║            === Thank  You ===            ║\n");
	fprintf(pfile, "║                                          ║\n");
	fprintf(pfile, "╚══════════════════════════════════════════╝\n");
	fclose(pfile);
}

void drawcontent(){
	int ctr, i;
	int filteram=0, disp=0;
	printf("    Filters: ");
	if(filterlist.active[0]){printf("Name (%s)", filterlist.name);filteram++;}
	if(filterlist.active[1]){printf("%sAuthor (%s)", filteram?", ":"", filterlist.auth);filteram++;}
	if(filterlist.active[2]){printf("%sGenre (%s)", filteram?", ":"", filterlist.genre);filteram++;}
	if(filterlist.active[3]){printf("%sPublisher (%s)", filteram?", ":"", filterlist.pub);filteram++;}
	if(filterlist.active[4]){printf("%sPublished Year (%d-%d)", filteram?", ":"", filterlist.yearbot, filterlist.yeartop);filteram++;}
	if(filterlist.active[5]){printf("%sStock (%d-%d)", filteram?", ":"", filterlist.stockbot, filterlist.stocktop);filteram++;}
	if(filterlist.active[6]){printf("%sPrice ($ %.2f-$ %.2f)", filteram?", ":"", filterlist.pricebot, filterlist.pricetop);filteram++;}
	if(filteram==0)printf("None");
	printf("\n");
	printf("  ╔═════╦═════════════════════════════════╦══════════════════════╦═════════════╦═══════════════════╦══════╦═══════╦═══════════╗\n");
	printf("  ║ BID ║            Book Name            ║        Author        ║    Genre    ║     Publisher     ║ Year ║ Stock ║   Price   ║\n");
	printf("  ╠═════╬═════════════════════════════════╬══════════════════════╬═════════════╬═══════════════════╬══════╬═══════╬═══════════╣\n");
	for(ctr=page*maxitemperpage; ctr<showbookamount&&ctr<(page+1)*maxitemperpage; ctr++){
		i=showbook[ctr];
		printf("  ║ %03d ║ %-31s ║ %-20s ║ %-11s ║ %-17s ║ %-4d ║  %-3d%s ║ $%8.2f ║\n", showbook[ctr]+1, booklist[i].name, booklist[i].auth, booklist[i].genre, booklist[i].pub, booklist[i].year, booklist[i].stock, booklist[i].stock<=lowstocklimit?"‼":" ", booklist[i].price);
		disp++;
	}
	printf("  ╚═════╩═════════════════════════════════╩══════════════════════╩═════════════╩═══════════════════╩══════╩═══════╩═══════════╝\n");
	char sortstr[20], adstr[5];
	if(sortby/10==1)strcpy(sortstr, "Name");
	else if(sortby/10==2)strcpy(sortstr, "Author");
	else if(sortby/10==3)strcpy(sortstr, "Genre");
	else if(sortby/10==4)strcpy(sortstr, "Publisher");
	else if(sortby/10==5)strcpy(sortstr, "Year");
	else if(sortby/10==6)strcpy(sortstr, "Stock");
	else if(sortby/10==7)strcpy(sortstr, "Price");
	else if(sortby/10==8)strcpy(sortstr, "Book ID");
	if(sortby%10==0)strcpy(adstr, "▲");
	else strcpy(adstr, "▼");
	printf("    Sort by: %-10s (%s)\t\t\t\t    [ %2d / %2d ]\t\t\t       Showing %4d out of %4d items\n", sortstr, adstr, page+1, maxpage+1, disp, bookamount);
}

void op1(){
	system("mode con: cols=65 lines=40");
	cartamount=0;
	int ctr, index, amount;
	char add='y', com, prt;
	float total=0;
	while(1){
		system("cls");drawheaders();
		printf("\n\t\t\tBook Purchase List\n");
		printf("\t  Cart (%d)\n", cartamount);
		for(ctr=0; ctr<cartamount; ctr++){
			printf("\t%s\n", booklist[cart[ctr].index].name);
			printf("\t%7d x $%8.2f\t\t= $%10.2f\n", cart[ctr].amount, booklist[cart[ctr].index].price, (float)cart[ctr].amount*booklist[cart[ctr].index].price);
		}
		if(cartamount!=0){
			printf("\t______________________________________________+\n");
			printf("\t  Total                           $%10.2f\n", total);
			printf("\t  Tax                             $%10.2f\n", total*.1);
			printf("\t  GRAND TOTAL                     $%10.2f\n", total*1.1);
		}
		printf("\n");
		if(lower(add)!='y')break;
		printf("\tAdd a book to cart\n");
		printf("\tSelect a Book ID (BID): ");
		if(parseint(&index, 3, 0)==-1){if(cartamount){add='n';continue;}return;}printf("\n");
		if(index<=0)break;index--;
		if(index>bookamount)continue;
		printf("\tSelected book name    : %s\n", booklist[index].name);
		printf("\tSelected book price   : $%8.2f\n", booklist[index].price);
		printf("\tSelected book stock   : %d\n", booklist[index].stock);
		printf("\tAmount                : ");
		if(parseint(&amount, 3, 1)==-1){if(cartamount){add='n';continue;}return;}printf("\n");
		if(amount==0||amount>booklist[index].stock)continue;
		for(ctr=0; ctr<cartamount; ctr++)if(index==cart[ctr].index)break;
		if(ctr==cartamount&&amount>0){
			struct cartitem newcartitem;
			newcartitem.index=index;
			newcartitem.amount=amount;
			cart[cartamount++]=newcartitem;
			total+=(float)amount*booklist[index].price;
			booklist[cart[ctr].index].stock-=cart[ctr].amount;
		}
		else{
			if(amount<0&&amount*-1>cart[ctr].amount)amount=-1*cart[ctr].amount;
			cart[ctr].amount+=amount;
			total+=(float)amount*booklist[index].price;
			booklist[cart[ctr].index].stock-=amount;
			if(cartamount&&cart[ctr].amount<=0){
				for(ctr; ctr<cartamount; ctr++)cart[ctr]=cart[ctr+1];
				cartamount--;
			}
		}
		printf("\tAdd another book (Y/N)? ");
		if(parsebet(&add, "yn")==-1){add='N';printf("N");}printf("\n");
	}
	if(cartamount==0)return;
	printf("\tCommit purchase  (Y/N)? ");
	if(parsebet(&com, "yn")==-1){com='N';printf("N");}printf("\n");
	if(lower(com)=='y'){printinvoice();savedata();}
	else for(ctr=0; ctr<cartamount; ctr++)booklist[cart[ctr].index].stock+=cart[ctr].amount;
}

void op2(){
	system("mode con: cols=65 lines=17");
	struct book newbook;
	char save, another='Y';
	int ctr;
	while(lower(another)=='y'){
		system("cls");drawheaders();
		printf("\n\tAdd a book entry\n");
		printf("\tBook name             : \n");
		printf("\tBook author           : \n");
		printf("\tBook genre            : \n");
		printf("\tBook publisher        : \n");
		printf("\tBook published year   : \n");
		printf("\tBook stock            : \n");
		printf("\tBook price            : $\n");
		if(parselocstr(newbook.name, 32, 6, "", 31)==-1)return;
		for(ctr=0; ctr<bookamount; ctr++){
			if(!strcmpi(booklist[ctr].name, newbook.name)){
				gotoxy(0, 13);
				printf("\tError, existing book have same name.\n");
				printf("\tReturn to main menu.\n");
				getche();return;
			}
		}
		if(parselocstr(newbook.auth, 32, 7, "", 20)==-1)return;
		if(parselocstr(newbook.genre, 32, 8, "", 11)==-1)return;
		if(parselocstr(newbook.pub, 32, 9, "", 17)==-1)return;
		if(parselocint(&newbook.year, 32, 10, 4, 0)==-1)return;
		char yr[10];
		time_t now=time(0);
		struct tm timeinfo=*localtime(&now);
		strftime(yr, sizeof(yr), "%Y", &timeinfo);
		int currentyear=(yr[0]-'0')*1000+(yr[1]-'0')*100+(yr[2]-'0')*10+(yr[3]-'0')*1;
		if(newbook.year>currentyear){
			gotoxy(0, 13);
			printf("\tError, year cannot exceed current year (%d).\n", currentyear);
			printf("\tReturn to main menu.\n");
			getche();return;
		}
		if(parselocint(&newbook.stock, 32, 11, 3, 0)==-1)return;
		if(parselocflt(&newbook.price, 33, 12, 8, 2)==-1)return;
		printf("\n\tAre you sure you want to save the book (Y/N)? ");
		if(parsebet(&save, "yn")==-1)return; printf("\n");
		if(lower(save)=='y'){booklist[bookamount++]=newbook;savedata();filterbooks();sortbooks();}
		printf("\tDo you want to add another book        (Y/N)? ");
		if(parsebet(&another, "yn")==-1)return; printf("\n");
	}
}

void op3(){
	while(1){
		system("mode con: cols=65 lines=17");
		system("cls");drawheaders();
		int sel, ctr;
		printf("\n");
		printf("\tSelect Book ID        : ");
		if(parseint(&sel, 3, 0)==-1)return; printf("\n");
		if(!sel)return;
		else sel--;
		if(sel<0||sel>bookamount){printf("\tInvalid Book ID.");getche();continue;}
		system("cls");drawheaders();
		char ed='n', com='n', op;
		printf("\tName           : %s\n", booklist[sel].name);
		printf("\tAuthor         : %s\n", booklist[sel].auth);
		printf("\tGenre          : %s\n", booklist[sel].genre);
		printf("\tPublisher      : %s\n", booklist[sel].pub);
		printf("\tPublished Year : %d\n", booklist[sel].year);
		printf("\tStock          : %d\n", booklist[sel].stock);
		printf("\tPrice          : $ %.2f\n", booklist[sel].price);
		printf("\n\tAre you sure you want to edit this book (Y/N)? ");
		if(parsebet(&ed, "yn")==-1)return; printf("\n");
		if(lower(ed)=='y'){
			system("mode con: cols=65 lines=28");
			system("cls");drawheaders();
			printf("\tName           : %s\n", booklist[sel].name);
			printf("\tAuthor         : %s\n", booklist[sel].auth);
			printf("\tGenre          : %s\n", booklist[sel].genre);
			printf("\tPublisher      : %s\n", booklist[sel].pub);
			printf("\tPublished Year : %d\n", booklist[sel].year);
			printf("\tStock          : %d\n", booklist[sel].stock);
			printf("\tPrice          : $ %.2f\n", booklist[sel].price);
			printf("\n");
			printf("\t1. Edit name\n");
			printf("\t2. Edit author\n");
			printf("\t3. Edit genre\n");
			printf("\t4. Edit publisher\n");
			printf("\t5. Edit year\n");
			printf("\t6. Edit stock\n");
			printf("\t7. Edit price\n");
			printf("\t8. Delete book entry\n");
			printf("\t0. Cancel\n");
			printf("\n");
			printf("\tOperation             : ");
			if(parsebet(&op, "012345678")==-1)return; printf("\n\n");
			char changes[100], changec;int changei;float changef;
			if(op=='0')continue;
			else if(op=='1'){
				printf("\tBook name             : ");
				if(parsestr(changes, "", 31)==-1)continue; printf("\n");
				for(ctr=0; ctr<bookamount; ctr++){
					if(!strcmpi(booklist[ctr].name, changes)){
						gotoxy(0, 25);
						printf("\tError, existing book have same name.\n");
						printf("\tReturn to main menu.\n");
						getche();return;
					}
				}
			}
			else if(op=='2'){
				printf("\tBook author           : ");
				if(parsestr(changes, "", 20)==-1)continue; printf("\n");
			}
			else if(op=='3'){
				printf("\tBook genre            : ");
				if(parsestr(changes, "", 11)==-1)continue; printf("\n");
			}
			else if(op=='4'){
				printf("\tBook publisher        : ");
				if(parsestr(changes, "", 17)==-1)continue; printf("\n");
			}
			else if(op=='5'){
				printf("\tBook published year   : ");
				if(parseint(&changei, 4, 0)==-1)continue; printf("\n");
				char yr[10];
				time_t now=time(0);
				struct tm timeinfo=*localtime(&now);
				strftime(yr, sizeof(yr), "%Y", &timeinfo);
				int currentyear=(yr[0]-'0')*1000+(yr[1]-'0')*100+(yr[2]-'0')*10+(yr[3]-'0')*1;
				if(changei>currentyear){
					gotoxy(0, 25);
					printf("\tError, year cannot exceed current year (%d).\n", currentyear);
					printf("\tReturn to main menu.\n");
					getche();return;
				}
			}
			else if(op=='6'){
				printf("\tBook stock            : ");
				if(parseint(&changei, 3, 0)==-1)continue; printf("\n");
			}
			else if(op=='7'){
				printf("\tBook price            : $");
				if(parseflt(&changef, 8, 2)==-1)continue; printf("\n");
			}
			printf("\tCommit changes (Y/N)? ");
			if(parsebet(&com, "yn")==-1)continue; printf("\n");
			if(lower(com)=='n')continue;
			else if(op=='1')strcpy(booklist[sel].name, changes);
			else if(op=='2')strcpy(booklist[sel].auth, changes);
			else if(op=='3')strcpy(booklist[sel].genre, changes);
			else if(op=='4')strcpy(booklist[sel].pub, changes);
			else if(op=='5')booklist[sel].year=changei;
			else if(op=='6')booklist[sel].stock=changei;
			else if(op=='7')booklist[sel].price=changef;
			else if(op=='8'){
				printf("\tAre you sure   (Y/N)? ");
				if(parsebet(&changec, "yn")==-1)return; printf("\n");
				if(changec=='n')continue;
				for(ctr=sel+1; ctr<bookamount; ctr++)booklist[ctr-1]=booklist[ctr];
				struct book emptybook;
				booklist[--bookamount]=emptybook;
				filterbooks();sortbooks();
			}
			savedata();
		}
	}
}

void op4(){
	system("mode con: cols=65 lines=17");
	system("cls");drawheaders();
	char op, ad;
	printf("\tSort\n");
	printf("\t\t1. Sort by name\n");
	printf("\t\t2. Sort by author\n");
	printf("\t\t3. Sort by genre\n");
	printf("\t\t4. Sort by publisher\n");
	printf("\t\t5. Sort by published year\n");
	printf("\t\t6. Sort by stock\n");
	printf("\t\t7. Sort by price\n");
	printf("\t\t8. Sort by ID\n");
	printf("\t\t0. Cancel\n");
	printf("\tOperation             : ");
	if(parsebet(&op, "012345678")==-1)return; printf("\n");
	if(op=='0')return;
	printf("\tAscending or descending (A/D)? ");
	if(parsebet(&ad, "ad")==-1)return; printf("\n");
	sortby=(op-'0')*10+(lower(ad)=='d'?1:0);
	filterbooks(); sortbooks();
}

void op5(){
	system("mode con: cols=65 lines=17");
	system("cls");drawheaders();
	int ctr;char op;
	printf("\tFilters               :\n");
	printf("\t\t1. Turn %s name filter\n", filterlist.active[0]?"off":"on");
	printf("\t\t2. Turn %s author filter\n", filterlist.active[1]?"off":"on");
	printf("\t\t3. Turn %s genre filter\n", filterlist.active[2]?"off":"on");
	printf("\t\t4. Turn %s publisher filter\n", filterlist.active[3]?"off":"on");
	printf("\t\t5. Turn %s published year filter\n", filterlist.active[4]?"off":"on");
	printf("\t\t6. Turn %s stock filter\n", filterlist.active[5]?"off":"on");
	printf("\t\t7. Turn %s price filter\n", filterlist.active[6]?"off":"on");
	printf("\t\t8. Clear all filter off\n");
	printf("\t\t0. Cancel\n");
	printf("\tOperation             : ");
	if(parsebet(&op, "123456780")==-1)return; printf("\n");
	if(op=='0')return;
	else if(op=='8'){for(ctr=0; ctr<7; ctr++)filterlist.active[ctr]=0;}
	else{
		filterlist.active[op-'0'-1]=!filterlist.active[op-'0'-1];
		if(filterlist.active[op-'0'-1]){
			if(op=='1'){
				printf("\tName Filter           : ");
				if(parsestr(filterlist.name, "", 31)==-1)return; printf("\n");
			}
			else if(op=='2'){
				printf("\tAuthor Filter         : ");
				if(parsestr(filterlist.auth, "", 20)==-1)return; printf("\n");
			}
			else if(op=='3'){
				printf("\tGenre Filter          : ");
				if(parsestr(filterlist.genre, "", 11)==-1)return; printf("\n");
			}
			else if(op=='4'){
				printf("\tPublisher Filter      : ");
				if(parsestr(filterlist.pub, "", 17)==-1)return; printf("\n");
			}
			else if(op=='5'){
				printf("\tYear Start Filter     : ");
				if(parseint(&filterlist.yearbot, 4, 0)==-1)return; printf("\n");
				printf("\tYear End   Filter     : ");
				if(parseint(&filterlist.yeartop, 4, 0)==-1)return; printf("\n");
				if(filterlist.yeartop<filterlist.yearbot)swap(&filterlist.yeartop, &filterlist.yearbot);
			}
			else if(op=='6'){
				printf("\tStock Start Filter    : ");
				if(parseint(&filterlist.stockbot, 3, 0)==-1)return; printf("\n");
				printf("\tStock End   Filter    : ");
				if(parseint(&filterlist.stocktop, 3, 0)==-1)return; printf("\n");
				if(filterlist.stocktop<filterlist.stockbot)swap(&filterlist.stocktop, &filterlist.stockbot);
			}
			else if(op=='7'){
				printf("\tPrice Start Filter    : $");
				if(parseflt(&filterlist.pricebot, 8, 2)==-1)return; printf("\n");
				printf("\tPrice End   Filter    : $");
				if(parseflt(&filterlist.pricetop, 8, 2)==-1)return; printf("\n");
				if(filterlist.pricetop<filterlist.pricebot){
					filterlist.pricetop+=filterlist.pricebot;
					filterlist.pricebot=filterlist.pricetop-filterlist.pricebot;
					filterlist.pricetop=filterlist.pricetop-filterlist.pricebot;
				}
			}
		}
	}
	filterbooks(); sortbooks();
}

void op6(){
	system("mode con: cols=65 lines=17");
	system("cls");drawheaders();
	int max, lim, ctr;char op;
	printf("\tOptions               :\n");
	printf("\t\t1. Change background color\n");
	printf("\t\t2. Change text color\n");
	printf("\t\t3. Change maximum item per page\n");
	printf("\t\t4. Change low stock limit\n");
	printf("\t\t5. Reset to default option\n");
	printf("\t\t0. Cancel\n");
	printf("\tOperation               : ");
	if(parsebet(&op, "123450")==-1)return; printf("\n");
	if(op=='0')return;
	else if(op=='1'||op=='2'){
		system("cls");
		drawheaders();
		if(op=='1')printf("\tChange background color :\n");
		else printf("\tChange text color       :\n");
		printf("\t\t0. Black");printf("\t8. Gray\n");
		printf("\t\t1. Blue");printf("\t\t9. Light Blue\n");
		printf("\t\t2. Green");printf("\tA. Light Green\n");
		printf("\t\t3. Aqua");printf("\t\tB. Light Aqua\n");
		printf("\t\t4. Red");printf("\t\tC. Light Red\n");
		printf("\t\t5. Purple");printf("\tD. Light Purple\n");
		printf("\t\t6. Yellow");printf("\tE. Light Yellow\n");
		printf("\t\t7. White");printf("\tF. Bright White\n");
		char col;
		printf("\tOperation               : ");
		if(parsebet(&col, "0123456789abcdef")==-1)return; printf("\n");
		if(op=='1'){
			if(lower(col)==lower(color[7])){printf("\tBackground color cannot be the same as text color,\n\treturn to main menu.\n");getch();}
			else color[6]=lower(col);
		}
		else{
			if(lower(col)==lower(color[6])){printf("\tBackground color cannot be the same as text color,\n\treturn to main menu.\n");getch();}
			else color[7]=lower(col);
		}
	}
	else if(op=='3'){
		system("cls");drawheaders();
		printf("\tCurrently showing %d item per page.\n", maxitemperpage);
		printf("\t(Choose a positive integer between 5-40)\n");
		printf("\tChange maximum item per page to   : ");
		if(parseint(&max, 2, 0)==-1)return; printf("\n");
		if(max<0){printf("\tMaximum item per page cannot be negative,\n\treturn to main menu.\n");getch();}
		else if(max>=5&&max<=40){maxitemperpage=max;filterbooks();sortbooks();changewindowsize();}
		else {printf("\tInvalid input, return to main menu.\n");getch();}
	}
	else if(op=='4'){
		system("cls");drawheaders();
		printf("\tCurrently showing warning below %d items.\n", lowstocklimit);
		printf("\t(Choose a positive integer between 1-99)\n");
		printf("\tChange low stock warning limit to : ");
		if(parseint(&lim, 2, 0)==-1)return; printf("\n");
		if(lim<0){printf("\tLow stok warning cannot be negative,\n\treturn to main menu.\n");getch();}
		else if(lim>=1&&lim<=100){lowstocklimit=lim;}
		else {printf("\tInvalid input, return to main menu.\n");getch();}
	}
	else if(op=='5'){
		system("cls");drawheaders();
		char in;
		printf("\n\tAre you sure you want to reset option (Y/N)? ");
		if(parsebet(&in, "yn")==-1)return; printf("\n");
		if(lower(in)=='y'){
			color[6]='0';
			color[7]='f';
			maxitemperpage=25;
			lowstocklimit=5;
			changewindowsize();
			sortby=80;
			for(ctr=0; ctr<8; ctr++)filterlist.active[ctr]=0;
			filterbooks();
			sortbooks();
		}
	}
	saveoption();
}

void op7(){
	system("mode con: cols=65 lines=17");
	system("cls");drawheaders();
	struct user newuser;
	int loc=0, ctr;
	char in, pass[50], con[50], com, type;
	printf("\tCreate new user\n");
	printf("\tOptions                   :\n");
	printf("\t\t1. Manager    (MNJ)\n");
	printf("\t\t2. Cashier    (CSH)\n");
	printf("\t\t3. User/Guest (USR)\n");
	printf("\tRole                      : \n");
	printf("\tUsername (case sensitive) : \n");
	printf("\tPassword (case sensitive) : \n");
	printf("\tConfirm  (case sensitive) : \n");
	if(parselocbet(&type, 36, 9, "123")==-1)return;
	if(parselocstr(newuser.name, 36, 10, "", 30)==-1)return;
	for(ctr=0; ctr<useramount; ctr++){
		if(!strcmpi(userlist[ctr].name, newuser.name)){
			gotoxy(0, 13);
			printf("\tError, existing user have same name.\n");
			printf("\tReturn to main menu.\n");
			getche();return;
		}
	}
	if(parselocstr(pass, 36, 11, "•", 30)==-1)return;
	if(parselocstr(con, 36, 12, "•", 30)==-1)return;
	if(!strcmp(pass, con)){
		if(type=='1')strcpy(newuser.role, "MNJ");
		else if(type=='2')strcpy(newuser.role, "CSH");
		else if(type=='3')strcpy(newuser.role, "USR");
		strcpy(newuser.pass, pass);
		printf("\n\tCommit additional user (Y/N)? ");
		if(parsebet(&com, "yn")==-1)return; printf("\n");
		if(lower(com)=='y')userlist[useramount++]=newuser;
		saveuser();
	}
	else{printf("\tPassword doesn't match, return to main menu.\n");getch();}
}

void opq(){
	system("mode con: cols=65 lines=25");
	system("cls");drawheaders();
	printf("           Thank you for using this software. This software \n");
	printf("       is written for the purpose of completing \"Algorithms\n");
	printf("       and Programming\"  course. As per writing this version\n");
	printf("       of the software, the developers had spent 14 days of \n");
	printf("       and written 1000 lines of code. We are glad that you \n");
	printf("       enjoy using it.                                      \n");
	printf("                                                            \n");
	printf("           Thanks to:                                       \n");
	printf("             - Main Development Team                        \n");
	printf("               > William Darian   (00000010691)             \n");
	printf("               > Steven Ricad     (00000011140)             \n");
	printf("               > Maulana Ramadhan (00000012101)             \n");
	printf("             - Beta Testers                                 \n");
	printf("               > Nicholas Robert                            \n");
	printf("               > Viona Stella                               \n");
	printf("               > Richard Dharmawan                          \n");
	printf("               > Vincent Dharma Simanjaya                   \n");
	printf("               > Rama Sandjaya                              \n");
	printf("               > I. Fajar Prasetya                          \n");
	printf("               > Gionardy Goputra                           \n");
	getch();
}

int loadingscreen(){
	system("chcp 65001");
	system("title \"The Louvre Bookstore\"");
	system("mode con: cols=65 lines=14");
	int ctr;
	system("cls"); drawheaders();
	printf("\t\t                                \n");
	printf("\t\t            Loading...          \n");
	printf("\t\t        (                )      \n");
	printf("\t\t  ┌────────────────────────────┐\n");
	printf("\t\t  │                            │\n");
	printf("\t\t  └────────────────────────────┘\n");
	printf("\t\t  ┌────────────────────────────┐\n");
	printf("\t\t  │                            │\n");
	printf("\t\t  └────────────────────────────┘\n");
	for(ctr=0; ctr<112; ctr++){
		if(ctr%28==0){gotoxy(19+ctr%28,8);printf("                            ");}
		gotoxy(19+ctr%28,8);printf("▒");
		if(ctr==0){gotoxy(25,6);printf("  Initializing  ");gotoxy(19,11);printf("███████");}
		else if(ctr==27){gotoxy(25,6);printf("Loading Booklist");if(loaddata()==-1)return -1;gotoxy(26,11);printf("███████");}
		else if(ctr==55){gotoxy(25,6);printf("Loading  Options");if(loadoption()==-1)return -2;system(color);gotoxy(33,11);printf("███████");}
		else if(ctr==83){gotoxy(25,6);printf("   Finalizing   ");if(loadusers()==-1)return -3;gotoxy(40,11);printf("███████");}
		else Sleep(10);
	}
	return 0;
}

int login(){
	system("mode con: cols=65 lines=14");
	int ctr=0;
	char name[50]={0}, pass[50]={0}, in;
	system("cls"); drawheaders();
	printf("\n\t\t   (Press Esc anytime to quit)\n");
	printf("\t\t   (For guests: guest / guest)\n\n");
	printf("\t\tUsername       : \n");
	printf("\t\tPassword       : \n");
	if(parselocstr(name, 33, 8, "", 30)==-1)return-1;
	if(parselocstr(pass, 33, 9, "•", 30)==-1)return -1;
	for(ctr=0; ctr<useramount; ctr++){
		if(!strcmp(userlist[ctr].name, name)){
			if(!strcmp(userlist[ctr].pass, pass)){
				int res;
				sortby=80;
				for(res=0; res<8; res++)filterlist.active[res]=0;
				filterbooks();
				page=0;
				loggeduser=ctr;
				if(!strcmp(userlist[ctr].role, "ADM"))return 1;
				else if(!strcmp(userlist[ctr].role, "MNJ"))return 2;
				else if(!strcmp(userlist[ctr].role, "CSH"))return 3;
				else return 4;
			}
			else break;
		}
	}
	printf("\n\n\t    Wrong Username and Password combination!"); getch();
	return 0;
}

void drawmenu(int as){
	while(1){
		system(pp); system("cls"); system(color);
		drawheader(); drawcontent();
		char op;
		printf("\n");
		printf("\t\t\t\tSelect an operation   : (Logged in as %s, %s)\n", userlist[loggeduser].name, userlist[loggeduser].role);
		printf("\t\t\t\t\t<. Go to previous page");
		printf("\t\t>. Go to next page\n");
		if(as==1){
			printf("\t\t\t\t\t1. Make a purchase");
			printf("\t\t2. Add a book entry\n");
			printf("\t\t\t\t\t3. Select and edit an entry");
			printf("\t4. Sort books\n");
			printf("\t\t\t\t\t5. Add or remove filters");
			printf("\t6. Options\n");
			printf("\t\t\t\t\t7. Add new account");
			printf("\t\t0. Logout\n");
			printf("\t\t\t\tOperation             : ");
			if(parsebet(&op, "<>12345670?")==-1)break;
			if(op=='0')return;
			else if(op=='<')page--;
			else if(op=='>')page++;
			else if(op=='1')op1();
			else if(op=='2')op2();
			else if(op=='3')op3();
			else if(op=='4')op4();
			else if(op=='5')op5();
			else if(op=='6')op6();
			else if(op=='7')op7();
			else if(op=='?')opq();
		}
		else if(as==2){
			printf("\t\t\t\t\t1. Make a purchase");
			printf("\t\t2. Add a book entry\n");
			printf("\t\t\t\t\t3. Select and edit an entry");
			printf("\t4. Sort books\n");
			printf("\t\t\t\t\t5. Add or remove filters");
			printf("\t6. Options\n");
			printf("\t\t\t\t\t0. Logout\n");
			printf("\t\t\t\tOperation             : ");
			if(parsebet(&op, "<>1234560?")==-1)break;
			if(op=='0')return;
			else if(op=='<')page--;
			else if(op=='>')page++;
			else if(op=='1')op1();
			else if(op=='2')op2();
			else if(op=='3')op3();
			else if(op=='4')op4();
			else if(op=='5')op5();
			else if(op=='6')op6();
			else if(op=='?')opq();
		}
		else if(as==3){
			printf("\t\t\t\t\t1. Make a purchase");
			printf("\t\t2. Sort books\n");
			printf("\t\t\t\t\t3. Add or remove filters");
			printf("\t0. Logout\n");
			printf("\t\t\t\tOperation             : ");
			if(parsebet(&op, "<>1230?")==-1)break;
			if(op=='0')return;
			else if(op=='<')page--;
			else if(op=='>')page++;
			else if(op=='1')op1();
			else if(op=='2')op4();
			else if(op=='3')op5();
			else if(op=='?')opq();
		}
		else if(as==4){
			printf("\t\t\t\t\t1. Sort books");
			printf("\t\t\t2. Add or remove filters\n");
			printf("\t\t\t\t\t0. Logout\n");
			printf("\t\t\t\tOperation             : ");
			if(parsebet(&op, "<>120?")==-1)break;
			if(op=='0')return;
			else if(op=='<')page--;
			else if(op=='>')page++;
			else if(op=='1')op4();
			else if(op=='2')op5();
		}
		if(page<0)page=maxpage;
		else if(page>maxpage)page=0;
		if(op!='<'&&op!='>')page=0;
	}
}

int main(){
	int data=loadingscreen(), loginas;
	if(data<0){errorscreen(data);return 0;}
	while(1){
		loginas=0;
		while(loginas==0)loginas=login();
		if(loginas==-1)return 0;
		drawmenu(loginas);
	}
	return 0;
}