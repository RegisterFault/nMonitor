%{
  char *intel_modelname;
%}
%option prefix="intel" outfile="intelparse.c"
%s model
%s intel
%s atom
%s celeron 
%s core2
%s core
%s corei3
%s corei5
%s corei7
%s pentium
%s pentium3
%s pentium4
%s xeon
%s endline
%%
<INITIAL>"model name" BEGIN(model);
<model>Intel\(R\) BEGIN(intel);
<model>"Celeron\(R\) Dual-Core CPU" BEGIN(celeron);
<intel>"CPU" ;
<intel>"Core\(TM\)" ;
<intel>"Atom\(TM\)" | 
"Atom\(TM\) CPU" |
"Atom\(TM\) Processor" BEGIN(atom);
<intel>"Celeron\(R\)" |
"Celeron\(R\) D CPU" |
"Celeron\(R\) M CPU" |
"Celeron\(R\) M processor" |
"Celeron\(R\) CPU" BEGIN(celeron);
<intel>"Core\(TM\)2 "("Solo "|"Duo "|"Quad "|"Extreme ")?"CPU" BEGIN(core2);
<intel>"Core\(TM\) Duo CPU" BEGIN(core2);
<intel>"Core\(TM\) CPU Q" BEGIN(core2);
<intel>"Core\(TM\) "("Solo CPU")? BEGIN(core);
<intel>"Core\(TM\) i3 CPU "[MU]? BEGIN(corei3);
<intel>"Core\(TM\) i5 CPU "[KMU]? BEGIN(corei5);
<intel>"Core\(TM\) i7 CPU "[KMULQXS]? BEGIN(corei7);
<intel>"Pentium\(R\)" |
"Pentium\(R\) 4 CPU" |
"Pentium\(R\) 4 - M CPU" |
"Pentium\(R\) 4 Mobile" BEGIN(pentium4);
<intel>"Pentium\(R\) CPU" |
"Pentium\(R\) D CPU" |
"Pentium\(R\) Dual CPU" |
"Pentium\(R\) Gold" |
"Pentium\(R\) M processor" |
"Pentium\(R\) Silver" BEGIN(pentium);
<model>"Pentium III "("CPU Family"|"Mobile CPU"|"CPU - M") BEGIN(pentium3);
<model>"Pentium III \(Coppermine\)" {
  snprintf(intel_modelname,256,"Pent III", yytext);
  BEGIN(endline);
}
<intel>"Processor" ;
<intel>"Xeon\(R\) CPU" |
"Xeon Phi\(TM\) CPU" |
"Xeon\(R\)"(" Bronze"|" Gold"|" Platinum"|" Silver") |
"Xeon\(R\)" |
"Xeon\(TM\) CPU" BEGIN(xeon);
<intel>" @ " ;
<intel>"processor" ;
<model>"Mobile Pentium MMX" {
  snprintf(intel_modelname,256,"Pentium MMX");
  BEGIN(endline);
}
<pentium>[^ \n\t]+ {
  snprintf(intel_modelname,256,"P1 %s",yytext);
  // printf("opteron %s\n", intel_modelname);
  BEGIN(endline);
}
<model>"Pentium II \(Deschtes\)" {
  snprintf(intel_modelname,256,"P2");
  BEGIN(endline);
}
<pentium3>[^ \n\t]+ {
  snprintf(intel_modelname,256,"P3 %s", yytext);
  BEGIN(endline);
}
<pentium4>[^ \n\t]+ {
  snprintf(intel_modelname,256,"P4 %s", yytext);
  BEGIN(endline);
}
<model>"Pentium\(R\) Dual-Core CPU" BEGIN(intel);
<intel>[^ \n\t]+ {
  snprintf(intel_modelname,256,"%s",yytext);
  //printf("Basic: %s\n", yytext);
  BEGIN(endline);
}
<atom>[^ \n\t]+ {
  snprintf(intel_modelname,256,"Atom %s",yytext);
  // printf("Sempron %s\n",yytext);
  BEGIN(endline);
}
<celeron>[^ \n\t]+ {
  snprintf(intel_modelname,256,"Cel %s",yytext);
  // printf("EPYC %s\n", yytext);
  BEGIN(endline);
}
<core2>[^ \n\t]+ {
  snprintf(intel_modelname,256,"Core2 %s",yytext);
  // printf("Threadripper %s\n", yytext);
  BEGIN(endline);
}
<core2>\n {
  snprintf(intel_modelname,256,"Core2 Quad",yytext);
  // printf("Threadripper %s\n", yytext);
  BEGIN(endline);
}
<core>[^ \n\t]+ {
  snprintf(intel_modelname,256,"%s",yytext);
  // printf("Ryzen %s\n", yytext);
  BEGIN(endline);
}
<corei3>[^ \n\t]+ {
  snprintf(intel_modelname,256,"i3-%s",yytext);
  // printf("Ryzen %s\n", yytext);
  BEGIN(endline);
}
<corei5>[^ \n\t]+ {
  snprintf(intel_modelname,256,"i5-%s",yytext);
  // printf("Ryzen %s\n", yytext);
  BEGIN(endline);
}
<corei7>[^ \n\t]+ {
  snprintf(intel_modelname,256,"i7-%s",yytext);
  // printf("Ryzen %s\n", yytext);
  BEGIN(endline);
}
<xeon>[^ \n\t]+ {
  snprintf(intel_modelname,256,"Xeon %s",yytext);
  // printf("turion %s\n", intel_modelname);
  BEGIN(endline);
}
<endline>[^\n]+ /* eat up the rest of the line*/ ;
<endline>\n BEGIN(INITIAL);
.|[ \t\n] ;
%%

int yywrap() {
  return 1;
}

char * fetch_intel_cpu_model(void){
  intel_modelname = malloc(256);
  FILE *fp = fopen("/proc/cpuinfo","r");
  if(fp){
      yyin = fp;
  } 
  yylex();
  return intel_modelname;
}
