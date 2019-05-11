#pragma once
#include <iostream>

using namespace std;

//https://www.piskelapp.com/ for gif to sprite

const int num_memes = 61;
const int num_gifs = 10;

char* memes[num_memes] = { "4head", "ayaya", "champ", "daijoubu", "dolan", "ehehe", "ezy", "feelsbadman",
"feelsgoodman", "feelsweirdman", "gachibass", "gachigasm", "hahaa", "handsup", "heavybreathing", "hyperbruh",
"hypers","kannanom", "kannapolice", "kappa", "klappa", "kreygasm", "lulw", "lul", "mikustare", "monkagun", "monkah",
"monkahmm", "monkamega", "monkaS", "monkaw", "nepsmug", "nyanpasu", "ohisee", "okaychamp", "omegalul", "peeposad",
"pepega","pepehands","pepelaugh","pepelmao","peperee","pepothink","pillowno","pillowyes","pogchamp",
"pog","pogey","poggers","pogu","pogyou","reeeee","residentsleeper","suchmeme","thisisfine","thonk","tuturu",
"waitwhat","weirdchamp","wesmart","kappa" };

char* meme_filenames[num_memes] = { "Emotes/4head.png", "Emotes/ayaya.png", "Emotes/champ.png", "Emotes/daijoubu.png", "Emotes/dolan.png", "Emotes/ehehe.png", "Emotes/ezy.png", "Emotes/feelsbadman.png",
"Emotes/feelsgoodman.png", "Emotes/feelsweirdman.png", "Emotes/gachibass.png", "Emotes/gachigasm.png", "Emotes/hahaa.png", "Emotes/handsup.png", "Emotes/heavybreathing.png", "Emotes/hyperbruh.png",
"Emotes/hypers.png","Emotes/kannanom.png", "Emotes/kannapolice.png", "Emotes/kappa.png", "Emotes/klappa.png", "Emotes/kreygasm.png", "Emotes/lulw.png", "Emotes/lul.jpg", "Emotes/mikustare.png", "Emotes/monkagun.png", "Emotes/monkah.png",
"Emotes/monkahmm.png", "Emotes/monkamega.png", "Emotes/monkaS.png", "Emotes/monkaw.png", "Emotes/nepsmug.png", "Emotes/nyanpasu.png", "Emotes/ohisee.png", "Emotes/okaychamp.png", "Emotes/omegalul.png", "Emotes/peeposad.png",
"Emotes/pepega.png","Emotes/pepehands.png","Emotes/pepelaugh.png","Emotes/pepelmao.png","Emotes/peperee.png","Emotes/pepothink.png","Emotes/pillowno.png","Emotes/pillowyes.png","Emotes/pogchamp.png",
"Emotes/pog.png","Emotes/pogey.png","Emotes/poggers.png","Emotes/pogu.png","Emotes/pogyou.png","Emotes/reeeee.png","Emotes/residentsleeper.png","Emotes/suchmeme.png","Emotes/thisisfine.png","Emotes/thonk.png","Emotes/tuturu.png",
"Emotes/waitwhat.png","Emotes/weirdchamp.png","Emotes/wesmart.png","Emotes/kappa.png" };

char* gifs[num_gifs] = { "miku_flip", "comeatmebro", "dealwithit", "dieinafire", "dumbass", 
"fml", "panic", "smug", "spaz", "thumbsup" };

char* gif_filenames[num_gifs] = { "gifs/miku_flip.png", "gifs/comeatmebro.png", "gifs/dealwithit.png", 
"gifs/dieinafire.png", "gifs/dumbass.png", "gifs/fml.png", "gifs/panic.png", 
"gifs/smug.png", "gifs/spaz.png", "gifs/thumbsup.png" };


int gif_width[num_gifs] = { 245,400,446,400,504,480,269,498,128,500 };

int gif_height[num_gifs] = { 245,225,228,225,240,270,270,285,128,281 };

long int gif_total_height[num_gifs] = { 7350,8775,10656,2250,16560,59940,3240,4845,1536,11802 };