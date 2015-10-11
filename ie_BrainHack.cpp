/*
 * Author: Ramith Udara Hettiarachchi <ceo@ruhsoft.com>
 * This code is a part of a eclipse project, so make a new project(Intel IoT C++), copy this code & compile..then run on intel edison..
 */

/*
 * C++ Consider a row of n coins of values v(1) ... v(n), where n is even. We play a game against an opponent by alternating turns.
 * In each turn, a player selects either the first or last coin from the row, removes it from the row permanently, and receives the value of the coin.
 *
 * Now two players are the Intel Edison User & The AI ;) (nope the dynamic program created by me)
 *
 *
 * Grove LCD, is connected to I2C
 * Grove Touch, D2
 * Grove Rotary Angle, A1
 * Buzzer attached to D5
 *
 *
 * Additional Note : I created this program for International Olympiad of Informatics
 * 					 selection test problem.. finally thought of running it on Intel Edison with some buttons ;)
 *
 *You have to play with the intel edison!
 *
 * Copyright (c) Ramith Hettiarachchi.
 */

//starting coding
#include "mraa.hpp"
#include <cstdlib> //
#include <time.h>  // used for generating random numbers
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "buzzer.h"
#include <grove.h>
#include <sstream>
#include <vector>
#include <jhd1313m1.h>

upm::Jhd1313m1 *lcd = new upm::Jhd1313m1(0, 0x3E, 0x62); //grove lcd
mraa::Aio* a_pin = new mraa::Aio(1);	//declare Rotary angle
upm::GroveButton* button = new upm::GroveButton(2);  //Grove touch


int pcscore=0;  //the score of intel edison
int myscore=0;  //your/user's score
using namespace std;

vector<int> arr;
bool isans[100][100]={false};
int ans[100][100];
string mymove;

int chord[] = { DO, RE, MI, FA, SOL, LA, SI, DO};
int ch=0;


void YouLostMusic(){


	  int chord[] = { MI, SOL, LA, SOL, SOL, RE, RE, DO, MI,MI,SOL,LA,SI,LA,SOL,MI,MI,RE,DO,RE,DO,DO };
	  int beats[]={ 120000, 120000, 700000, 300000, 1200000, 300000,300000,300000,1000000,300000,300000,300000,300000,300000,300000,300000,300000,300000,300000,700000,300000,1000000};

	  upm::Buzzer* sound = new upm::Buzzer(5);
	  std::cout << sound->name() << std::endl;

	    for (int chord_ind = 0; chord_ind <=21; chord_ind++) {
	      std::cout << sound->playSound(chord[chord_ind], beats[chord_ind]) << std::endl;
	      usleep(100000);
	  }
	  delete sound;
}
void YouWonMusic(){
	  int chord[] = {SOL,FA,SOL,FA,FA,FA,SOL,LA,SOL,FA,SOL,FA,1855,1855,1855,1855,LA,FA};

	  int qu=220000;
	  int beats[] = {  qu, qu, qu, 700000, qu, qu,qu,qu,qu,700000,550000,320000,700000,qu,qu,qu,600000,600000};
	  upm::Buzzer* sound = new upm::Buzzer(5);
	  std::cout << sound->name() << std::endl;

	  for (int chord_ind = 0; chord_ind <sizeof(chord)/sizeof(chord[0]); chord_ind++) {
	      std::cout << sound->playSound(chord[chord_ind], beats[chord_ind]) << std::endl;
	      usleep(100000);
	  }
	  delete sound;
}
void updatelcd(){
	lcd->setCursor(1,0);		//printing your score..
	lcd->write("YOU: ");
	stringstream you;
	you<<myscore;
	lcd->write(you.str());

	lcd->setCursor(1,10);		//printing my score..
	//lcd->write("                ");
	lcd->write("ME: ");

	stringstream me;
	me<<pcscore;
	lcd->write(me.str());

	cout<<"updates LCD"<<endl;
}

string userinput(){
	upm::Buzzer* sound = new upm::Buzzer(5);

	bool gotit=false;
	int i=0;

	sleep(1);
	std::cout<<"reading..  "<<endl;
	while(!gotit){

//		std::cout<<" , "<<a_pin->read();

		if(button->value()==1){
			i=a_pin->read();gotit=true;
			std::cout<<"confirmed... "<<i<<std::endl;
		}
		sleep(1);
	}

	sound->playSound(chord[ch], 300000);ch++;
    usleep(100000);

		  delete sound;
	if(i>500)return "RIGHT";
	else return "LEFT";
}

int get(int i,int j,string x){  ////the main logic -_- dynamic programming
    if(i>j || i==j)return 0;
    int d;

    int left=min(get(i+1,j-1,x),get(i+2,j,x));
    int right=min(get(i+1,j-1,x),get(i,j-2,x));
    d=max(left+arr[i],right+arr[j]);

    if(left+arr[i]>right+arr[j]){
        mymove="LEFT";		//updates move (globle var)
        ans[i][j]=0;
        isans[i][j]=true;

    }else{
        mymove="RIGHT";		//updates move (globle var)
        ans[i][j]=1;
        isans[i][j]=true;
    }

return d;
}

void lit(int x,bool on){  //function used to lit an LED
	 mraa::Gpio* d_pin = new mraa::Gpio(x);
	 d_pin->dir(mraa::DIR_OUT);
	 d_pin->write(on);
}

int main(){
	int N=8;		//game size
	int spoint=0;
	int epoint=N-1;
	string op;

	int ledpos[]={4,6,7,8,9,10,11,12}; ///the led's are connected to these pins from left->right

	for(int i=4;i<=12;i++)lit(i,0); ///switch off all the LED's before starting

	srand (time(NULL));
	stringstream prob;

	for(int j=0;j<8;j++){
		int k=rand() % 9 + 1;
		arr.push_back(k);
		prob<<" ";
		prob<<k;
	}

	cout<<prob.str()<<endl;
	lcd->setCursor(0,0);


	//lcd->write(" 1 3 6 4 2 3 7 4");	//print the nums on lcd

	lcd->write(prob.str());

	//lcd->setCursor(1,0);
	//	lcd->write("Make your move..");	//make your move

cout<<"game started _ below will be your's move"<<endl;

	for(int i=0;i<N/2;i++){

		  op=userinput();  //got users input...
		  cout<<"okay, you confirmed.. "<<op<<endl;

		  if(op=="LEFT") {
			   myscore+=arr[spoint];
			   lit(ledpos[spoint],1);
			   spoint+=1;
		  }else{
			    myscore+=arr[epoint];
			    lit(ledpos[epoint],1);
			    epoint-=1;
		  }
		  updatelcd();

	    get(spoint,epoint,"$");	//requesting move from dp function

	    if(mymove=="LEFT"){
	        pcscore+=arr[spoint];
	        spoint+=1;
	        cout<<"MACHINE : LEFT"<<endl;
	    }else{
	        pcscore+=arr[epoint];
	        epoint-=1;
	        cout<<"MACHINE : RIGHT"<<endl;
	    }
	    updatelcd();

	}

	if(myscore>pcscore)cout<<"\nYou Won !!!\n";
	else if(myscore==pcscore) cout<<"\nMATCH EQUAL !!!\n";
	else cout<<"\nYOU LOST !!!\n";

	cout<<"\nYOUR SCORE IS : "<<myscore<<endl;
	cout<<"PC SCORE IS : " <<pcscore<<endl;



if(myscore>pcscore){
	YouWonMusic();
	YouWonMusic(); ///play it twice!
}else {
	YouLostMusic();
	YouLostMusic();
}

	printf("Sleeping for 5 seconds\n");
	sleep(5);


	return MRAA_SUCCESS;
}
