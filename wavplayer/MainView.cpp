#include <gui/main_screen/MainView.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include "fatfs.h"
#include <iostream>
#include <stdio.h>

using namespace std;

extern char Files[4][25];
extern int playstate;

MainView::MainView()
{
}

void MainView::setupScreen()
{
}

void MainView::tearDownScreen()
{
}

void MainView::listprint()
{
}

void MainView::playmusic()
{
	if(playstate == 0) playstate = 1;
	else if(playstate == 1) playstate = 0;
	else playstate = 0;
}

